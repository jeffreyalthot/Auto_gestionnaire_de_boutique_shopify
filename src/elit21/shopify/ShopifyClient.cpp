#include "elit21/shopify/ShopifyClient.h"

#include "elit21/shopify/graphql/ShopifyUserErrorParser.h"
#include "elit21/shopify/ShopifyIdempotencyKey.h"
#include "elit21/shopify/ShopifyInventoryWritePlanner.h"

#include <algorithm>
#include <cmath>
#include <utility>

namespace elit21 {
namespace {

Json sourceMetafields(const std::string& product_id, const std::string& sku_id) {
    Json fields = Json::array();
    for (const auto& pair : std::vector<std::pair<std::string, std::string>>{
             {"aliexpress_product_id", product_id}, {"aliexpress_sku_id", sku_id}}) {
        Json field = Json::object();
        field.set("namespace", "elit21");
        field.set("key", pair.first);
        field.set("type", "single_line_text_field");
        field.set("value", pair.second);
        fields.push(field);
    }
    return fields;
}

std::vector<ShopifyProductVariantCreate> normalizedVariants(const ShopifyProductCreate& product) {
    if (!product.variants.empty()) return product.variants;
    ShopifyProductVariantCreate variant;
    variant.option_name = "Title";
    variant.option_value = product.source_sku_id.empty() ? "Default" : product.source_sku_id;
    variant.sku = product.sku;
    variant.source_product_id = product.source_product_id;
    variant.source_sku_id = product.source_sku_id;
    variant.price_cad = product.price_cad;
    variant.supplier_cost_cad = product.supplier_cost_cad;
    variant.inventory = product.inventory;
    return {variant};
}

} // namespace

ShopifyClient::ShopifyClient(ShopifyConfig config, HttpClient& http)
    : config_(std::move(config)),
      http_(http),
      transport_(http_, config_.graphqlEndpoint(), config_.access_token) {}

Result<Json> ShopifyClient::graphql(const std::string& query, const Json& variables, int max_attempts) {
    if (config_.shop.empty() || config_.access_token.empty()) {
        return Result<Json>::failure("Shopify credentials are missing");
    }
    auto response = transport_.executeRaw(query, variables, max_attempts);
    if (!response) return Result<Json>::failure(response.error());
    Json root = Json::object();
    root.set("data", response.value().data());
    root.set("extensions", response.value().extensions());
    return Result<Json>::success(std::move(root));
}

shopify::graphql::GraphqlTransportMetrics ShopifyClient::apiMetrics() const {
    return transport_.metrics();
}

Result<void> ShopifyClient::ensureNoUserErrors(const Json& payload, const std::string& operation) const {
    const auto errors = shopify::graphql::ShopifyUserErrorParser::parse(payload);
    if (!errors.empty()) {
        return Result<void>::failure(operation + ": " + shopify::graphql::ShopifyUserErrorParser::join(errors));
    }
    return Result<void>::success();
}

Result<void> ShopifyClient::healthCheck() {
    auto response = graphql(
        "query ShopHealth { shop { id name myshopifyDomain currencyCode primaryDomain { url } } }",
        Json::object());
    return response ? Result<void>::success() : Result<void>::failure(response.error());
}

CustomerOrder ShopifyClient::parseOrderNode(const Json& node) const {
    CustomerOrder order;
    order.shopify_order_id = node.getString("id");
    order.order_name = node.getString("name");
    order.email = node.getString("email");
    order.phone = node.getString("phone");
    const auto total = node.get("currentTotalPriceSet").get("shopMoney");
    order.total_cad = total.getNumber("amount", 0.0);
    order.currency = total.getString("currencyCode", "CAD");

    const auto address = node.get("shippingAddress");
    order.shipping_address.first_name = address.getString("firstName");
    order.shipping_address.last_name = address.getString("lastName");
    order.shipping_address.company = address.getString("company");
    order.shipping_address.address1 = address.getString("address1");
    order.shipping_address.address2 = address.getString("address2");
    order.shipping_address.city = address.getString("city");
    order.shipping_address.province = address.getString("province");
    order.shipping_address.province_code = address.getString("provinceCode");
    order.shipping_address.country_code = address.getString("countryCodeV2", "CA");
    order.shipping_address.postal_code = address.getString("zip");
    order.shipping_address.phone = address.getString("phone");

    const auto edges = node.get("lineItems").get("edges");
    for (std::size_t index = 0; index < edges.size(); ++index) {
        const auto item = edges.at(index).get("node");
        OrderLine line;
        line.shopify_line_id = item.getString("id");
        line.title = item.getString("title");
        line.quantity = item.getInt("currentQuantity");
        line.sku = item.getString("sku");
        const auto variant = item.get("variant");
        line.shopify_variant_id = variant.getString("id");
        const auto price = item.get("originalUnitPriceSet").get("shopMoney");
        line.unit_price_cad = price.getNumber("amount", 0.0);
        const auto metafields = variant.get("metafields").get("edges");
        for (std::size_t field_index = 0; field_index < metafields.size(); ++field_index) {
            const auto metafield = metafields.at(field_index).get("node");
            const auto key = metafield.getString("key");
            if (key == "aliexpress_product_id") line.aliexpress_product_id = metafield.getString("value");
            if (key == "aliexpress_sku_id") line.aliexpress_sku_id = metafield.getString("value");
        }
        order.lines.push_back(std::move(line));
    }
    order.raw_json = node.dump();
    return order;
}

Result<ShopifyOrderPage> ShopifyClient::fetchOpenPaidOrdersPage(int first,
                                                                const std::string& after_cursor) {
    Json variables = Json::object();
    variables.set("first", std::clamp(first, 1, 250));
    if (!after_cursor.empty()) variables.set("after", after_cursor);
    const std::string query = R"graphql(
query PaidUnfulfilledOrders($first:Int!,$after:String){
  orders(first:$first,after:$after,query:"financial_status:paid fulfillment_status:unfulfilled",sortKey:CREATED_AT){
    pageInfo{hasNextPage endCursor}
    edges{node{
      id name email phone
      currentTotalPriceSet{shopMoney{amount currencyCode}}
      shippingAddress{firstName lastName company address1 address2 city province provinceCode countryCodeV2 zip phone}
      lineItems(first:250){edges{node{
        id title sku currentQuantity originalUnitPriceSet{shopMoney{amount currencyCode}}
        variant{id metafields(first:10,namespace:"elit21"){edges{node{key value}}}}
      }}}
    }}
  }
})graphql";
    auto response = graphql(query, variables);
    if (!response) return Result<ShopifyOrderPage>::failure(response.error());

    ShopifyOrderPage page;
    const auto connection = response.value().get("data").get("orders");
    const auto edges = connection.get("edges");
    page.orders.reserve(edges.size());
    for (std::size_t index = 0; index < edges.size(); ++index) {
        auto order = parseOrderNode(edges.at(index).get("node"));
        if (order.valid()) page.orders.push_back(std::move(order));
    }
    const auto page_info = connection.get("pageInfo");
    page.has_next_page = page_info.getBool("hasNextPage", false);
    page.end_cursor = page_info.getString("endCursor");
    if (!page.cursorConsistent()) {
        return Result<ShopifyOrderPage>::failure("Shopify returned hasNextPage without an endCursor");
    }
    return Result<ShopifyOrderPage>::success(std::move(page));
}

Result<std::vector<CustomerOrder>> ShopifyClient::fetchOpenPaidOrders(int maximum_orders, int max_pages) {
    maximum_orders = std::clamp(maximum_orders, 1, 2500);
    max_pages = std::clamp(max_pages, 1, 50);
    std::vector<CustomerOrder> orders;
    orders.reserve(static_cast<std::size_t>(maximum_orders));
    std::string cursor;
    for (int page_index = 0; page_index < max_pages && static_cast<int>(orders.size()) < maximum_orders; ++page_index) {
        const int page_size = std::min(250, maximum_orders - static_cast<int>(orders.size()));
        auto page = fetchOpenPaidOrdersPage(page_size, cursor);
        if (!page) return Result<std::vector<CustomerOrder>>::failure(page.error());
        for (auto& order : page.value().orders) orders.push_back(std::move(order));
        if (!page.value().has_next_page) break;
        if (page.value().end_cursor.empty() || page.value().end_cursor == cursor) {
            return Result<std::vector<CustomerOrder>>::failure("Shopify order pagination cursor did not advance");
        }
        cursor = page.value().end_cursor;
    }
    return Result<std::vector<CustomerOrder>>::success(std::move(orders));
}

Json ShopifyClient::variantInput(const ShopifyProductVariantCreate& variant) {
    Json input = Json::object();
    input.set("price", variant.price_cad);
    input.set("taxable", variant.taxable);
    input.set("inventoryPolicy", "DENY");

    Json option_values = Json::array();
    Json option = Json::object();
    option.set("name", variant.option_value);
    option.set("optionName", variant.option_name.empty() ? "Title" : variant.option_name);
    option_values.push(option);
    input.set("optionValues", option_values);

    Json inventory_item = Json::object();
    inventory_item.set("sku", variant.sku);
    inventory_item.set("tracked", true);
    inventory_item.set("requiresShipping", true);
    inventory_item.set("cost", variant.supplier_cost_cad);
    input.set("inventoryItem", inventory_item);
    input.set("metafields", sourceMetafields(variant.source_product_id, variant.source_sku_id));
    return input;
}

ShopifyVariantRef ShopifyClient::parseVariantRef(const Json& node) {
    ShopifyVariantRef value;
    value.id = node.getString("id");
    value.sku = node.getString("sku");
    value.title = node.getString("title");
    value.inventory_item_id = node.get("inventoryItem").getString("id");
    return value;
}

Result<std::string> ShopifyClient::createProduct(const ShopifyProductCreate& product) {
    if (!product.valid()) return Result<std::string>::failure("Shopify product input is incomplete");

    Json variables = Json::object();
    Json input = Json::object();
    input.set("title", product.title);
    input.set("descriptionHtml", product.description_html);
    input.set("vendor", product.vendor);
    input.set("productType", product.product_type);
    Json metafields = Json::array();
    Json source = Json::object();
    source.set("namespace", "elit21");
    source.set("key", "aliexpress_product_id");
    source.set("type", "single_line_text_field");
    source.set("value", product.source_product_id);
    metafields.push(source);
    input.set("metafields", metafields);
    variables.set("product", input);

    Json media = Json::array();
    for (const auto& image_url : product.image_urls) {
        if (image_url.rfind("https://", 0) != 0) continue;
        Json image = Json::object();
        image.set("originalSource", image_url);
        image.set("mediaContentType", "IMAGE");
        image.set("alt", product.title);
        media.push(image);
    }
    variables.set("media", media);

    const auto created = graphql(
        R"graphql(mutation ProductCreate($product:ProductCreateInput!,$media:[CreateMediaInput!]){
          productCreate(product:$product,media:$media){
            product{id variants(first:1){nodes{id sku title inventoryItem{id}}}}
            userErrors{field message}
          }
        })graphql",
        variables);
    if (!created) return Result<std::string>::failure(created.error());
    const auto payload = created.value().get("data").get("productCreate");
    auto no_errors = ensureNoUserErrors(payload, "productCreate");
    if (!no_errors) return Result<std::string>::failure(no_errors.error());
    const auto created_product = payload.get("product");
    const std::string product_id = created_product.getString("id");
    if (product_id.empty()) return Result<std::string>::failure("Shopify productCreate returned no product ID");

    const auto variants = normalizedVariants(product);
    if (variants.size() > 1) {
        auto added = createVariants(product_id, variants, true);
        if (!added) {
            return Result<std::string>::failure(
                "Product " + product_id + " created, but its variants failed: " + added.error());
        }
        return Result<std::string>::success(product_id);
    }

    const auto initial_nodes = created_product.get("variants").get("nodes");
    if (initial_nodes.size() == 0) {
        return Result<std::string>::failure("Shopify created a product without an initial variant");
    }
    const auto& desired = variants.front();
    Json update_variables = Json::object();
    update_variables.set("productId", product_id);
    Json variant_updates = Json::array();
    Json update = variantInput(desired);
    update.set("id", initial_nodes.at(0).getString("id"));
    variant_updates.push(update);
    update_variables.set("variants", variant_updates);
    const auto updated = graphql(
        R"graphql(mutation ProductVariantsBulkUpdate($productId:ID!,$variants:[ProductVariantsBulkInput!]!){
          productVariantsBulkUpdate(productId:$productId,variants:$variants){
            productVariants{id sku inventoryItem{id}} userErrors{field message}
          }
        })graphql",
        update_variables);
    if (!updated) return Result<std::string>::failure("Product created but variant update failed: " + updated.error());
    no_errors = ensureNoUserErrors(
        updated.value().get("data").get("productVariantsBulkUpdate"), "productVariantsBulkUpdate");
    if (!no_errors) return Result<std::string>::failure(no_errors.error());
    return Result<std::string>::success(product_id);
}

Result<std::vector<ShopifyVariantRef>> ShopifyClient::createVariants(
    const std::string& product_gid,
    const std::vector<ShopifyProductVariantCreate>& variants,
    bool remove_standalone_variant) {
    if (product_gid.empty()) return Result<std::vector<ShopifyVariantRef>>::failure("Shopify product ID is empty");
    if (variants.empty() || variants.size() > 2048) {
        return Result<std::vector<ShopifyVariantRef>>::failure("Shopify variant count must be between 1 and 2048");
    }
    for (const auto& variant : variants) {
        if (!variant.valid()) return Result<std::vector<ShopifyVariantRef>>::failure("Invalid Shopify variant input");
    }

    std::vector<ShopifyVariantRef> output;
    output.reserve(variants.size());
    constexpr std::size_t batch_size = 100;
    for (std::size_t offset = 0; offset < variants.size(); offset += batch_size) {
        Json variables = Json::object();
        variables.set("productId", product_gid);
        variables.set("strategy", offset == 0 && remove_standalone_variant
            ? "REMOVE_STANDALONE_VARIANT" : "DEFAULT");
        Json batch = Json::array();
        const auto end = std::min(variants.size(), offset + batch_size);
        for (std::size_t index = offset; index < end; ++index) batch.push(variantInput(variants[index]));
        variables.set("variants", batch);

        auto response = graphql(
            R"graphql(mutation ProductVariantsBulkCreate($productId:ID!,$variants:[ProductVariantsBulkInput!]!,$strategy:ProductVariantsBulkCreateStrategy){
              productVariantsBulkCreate(productId:$productId,variants:$variants,strategy:$strategy){
                productVariants{id title sku inventoryItem{id}} userErrors{field message}
              }
            })graphql",
            variables);
        if (!response) return Result<std::vector<ShopifyVariantRef>>::failure(response.error());
        const auto payload = response.value().get("data").get("productVariantsBulkCreate");
        auto no_errors = ensureNoUserErrors(payload, "productVariantsBulkCreate");
        if (!no_errors) return Result<std::vector<ShopifyVariantRef>>::failure(no_errors.error());
        const auto nodes = payload.get("productVariants");
        for (std::size_t index = 0; index < nodes.size(); ++index) {
            auto reference = parseVariantRef(nodes.at(index));
            if (reference.valid()) output.push_back(std::move(reference));
        }
    }
    if (output.size() != variants.size()) {
        return Result<std::vector<ShopifyVariantRef>>::failure(
            "Shopify created " + std::to_string(output.size()) + " variants out of " +
            std::to_string(variants.size()));
    }
    return Result<std::vector<ShopifyVariantRef>>::success(std::move(output));
}

Result<void> ShopifyClient::updateVariantPrice(const std::string& product_gid,
                                                const std::string& variant_gid,
                                                double price_cad) {
    return updateVariantPrices(product_gid, {{variant_gid, price_cad}});
}

Result<void> ShopifyClient::updateVariantPrices(
    const std::string& product_gid,
    const std::vector<ShopifyVariantPriceUpdate>& updates) {
    if (product_gid.empty()) return Result<void>::failure("Shopify product ID is empty");
    if (updates.empty()) return Result<void>::success();
    constexpr std::size_t batch_size = 100;
    for (std::size_t offset = 0; offset < updates.size(); offset += batch_size) {
        Json variables = Json::object();
        variables.set("productId", product_gid);
        Json variants = Json::array();
        const auto end = std::min(updates.size(), offset + batch_size);
        for (std::size_t index = offset; index < end; ++index) {
            if (!updates[index].valid()) return Result<void>::failure("Invalid Shopify price update");
            Json variant = Json::object();
            variant.set("id", updates[index].variant_id);
            variant.set("price", updates[index].price_cad);
            variants.push(variant);
        }
        variables.set("variants", variants);
        auto response = graphql(
            R"graphql(mutation UpdateVariantPrices($productId:ID!,$variants:[ProductVariantsBulkInput!]!){
              productVariantsBulkUpdate(productId:$productId,variants:$variants){userErrors{field message}}
            })graphql",
            variables);
        if (!response) return Result<void>::failure(response.error());
        auto valid = ensureNoUserErrors(
            response.value().get("data").get("productVariantsBulkUpdate"), "updateVariantPrices");
        if (!valid) return valid;
    }
    return Result<void>::success();
}

Result<void> ShopifyClient::updateInventory(const std::string& inventory_item_gid,
                                             const std::string& location_gid,
                                             int quantity) {
    ShopifyInventoryQuantityUpdate update;
    update.inventory_item_id = inventory_item_gid;
    update.quantity = std::max(0, quantity);
    return updateInventories(location_gid, {update});
}

Result<void> ShopifyClient::updateInventories(
    const std::string& location_gid,
    const std::vector<ShopifyInventoryQuantityUpdate>& updates) {
    return updateInventoriesIdempotent(
        location_gid,
        updates,
        shopify::ShopifyIdempotencyKey::random(),
        "elit21://inventory/synchronization",
        true);
}

Result<void> ShopifyClient::updateInventoriesIdempotent(
    const std::string& location_gid,
    const std::vector<ShopifyInventoryQuantityUpdate>& updates,
    const std::string& idempotency_key,
    const std::string& reference_document_uri,
    bool allow_unchecked) {
    auto batches = shopify::ShopifyInventoryWritePlanner::build(
        location_gid, updates, idempotency_key, reference_document_uri, allow_unchecked);
    if (!batches) return Result<void>::failure(batches.error());
    for (const auto& batch : batches.value()) {
        auto response = graphql(
            shopify::ShopifyInventoryWritePlanner::mutationDocument(),
            batch.variables,
            4);
        if (!response) return Result<void>::failure(response.error());
        auto valid = ensureNoUserErrors(
            response.value().get("data").get("inventorySetQuantities"),
            "inventorySetQuantities");
        if (!valid) return valid;
    }
    return Result<void>::success();
}


Result<std::vector<ShopifyInventoryQuantitySnapshot>>
ShopifyClient::inventoryQuantitiesAtLocation(
    const std::string& location_gid,
    const std::vector<std::string>& inventory_item_gids) {
    if (location_gid.empty()) {
        return Result<std::vector<ShopifyInventoryQuantitySnapshot>>::failure(
            "Shopify location ID is required");
    }
    if (inventory_item_gids.empty()) {
        return Result<std::vector<ShopifyInventoryQuantitySnapshot>>::success({});
    }
    std::vector<ShopifyInventoryQuantitySnapshot> output;
    output.reserve(inventory_item_gids.size());
    for (std::size_t offset = 0; offset < inventory_item_gids.size(); offset += 250) {
        Json ids = Json::array();
        const auto end = std::min(inventory_item_gids.size(), offset + 250);
        for (std::size_t index = offset; index < end; ++index) {
            ids.push(Json(json_object_new_string(inventory_item_gids[index].c_str()), true));
        }
        Json variables = Json::object();
        variables.set("ids", ids);
        variables.set("locationId", location_gid);
        const std::string query = R"graphql(
query InventoryQuantitiesAtLocation($ids:[ID!]!,$locationId:ID!){
  nodes(ids:$ids){
    ... on InventoryItem{
      id
      inventoryLevel(locationId:$locationId){
        id updatedAt
        quantities(names:["available"]){name quantity}
      }
    }
  }
})graphql";
        auto response = graphql(query, variables);
        if (!response) {
            return Result<std::vector<ShopifyInventoryQuantitySnapshot>>::failure(response.error());
        }
        const auto nodes = response.value().get("data").get("nodes");
        for (std::size_t index = 0; index < nodes.size(); ++index) {
            const auto node = nodes.at(index);
            const auto level = node.get("inventoryLevel");
            ShopifyInventoryQuantitySnapshot snapshot;
            snapshot.inventory_item_id = node.getString("id");
            snapshot.inventory_level_id = level.getString("id");
            snapshot.updated_at = level.getString("updatedAt");
            const auto quantities = level.get("quantities");
            for (std::size_t quantity_index = 0; quantity_index < quantities.size(); ++quantity_index) {
                const auto quantity = quantities.at(quantity_index);
                if (quantity.getString("name") == "available") {
                    snapshot.available = quantity.getInt("quantity", -1);
                    break;
                }
            }
            if (snapshot.valid()) output.push_back(std::move(snapshot));
        }
    }
    return Result<std::vector<ShopifyInventoryQuantitySnapshot>>::success(std::move(output));
}

Result<std::vector<ShopifyVariantRef>> ShopifyClient::productVariants(const std::string& product_gid) {
    const std::string query = R"graphql(
      query ProductVariantReferences($id: ID!) {
        product(id: $id) { variants(first: 250) { nodes { id title sku inventoryItem { id } } } }
      }
    )graphql";
    Json variables = Json::object();
    variables.set("id", product_gid);
    auto response = graphql(query, variables);
    if (!response) return Result<std::vector<ShopifyVariantRef>>::failure(response.error());
    const auto nodes = response.value().get("data").get("product").get("variants").get("nodes");
    std::vector<ShopifyVariantRef> output;
    output.reserve(nodes.size());
    for (std::size_t index = 0; index < nodes.size(); ++index) {
        auto value = parseVariantRef(nodes.at(index));
        if (value.valid()) output.push_back(std::move(value));
    }
    return Result<std::vector<ShopifyVariantRef>>::success(std::move(output));
}

Result<std::string> ShopifyClient::primaryLocationId() {
    const std::string query = R"graphql(
      query PrimaryLocation { locations(first: 50, includeInactive: false) { nodes { id name fulfillsOnlineOrders } } }
    )graphql";
    auto response = graphql(query, Json::object());
    if (!response) return Result<std::string>::failure(response.error());
    const auto nodes = response.value().get("data").get("locations").get("nodes");
    std::string fallback;
    for (std::size_t index = 0; index < nodes.size(); ++index) {
        const auto node = nodes.at(index);
        const auto id = node.getString("id");
        if (fallback.empty()) fallback = id;
        if (node.getBool("fulfillsOnlineOrders", false) && !id.empty()) {
            return Result<std::string>::success(id);
        }
    }
    if (fallback.empty()) return Result<std::string>::failure("No active Shopify location is available");
    return Result<std::string>::success(fallback);
}

Result<std::vector<ShopifyFulfillmentOrderRef>> ShopifyClient::fulfillmentOrders(
    const std::string& order_gid) {
    Json variables = Json::object();
    variables.set("id", order_gid);
    auto response = graphql(
        R"graphql(query FulfillmentOrders($id:ID!){
          order(id:$id){fulfillmentOrders(first:100){nodes{id status requestStatus}}}
        })graphql",
        variables);
    if (!response) return Result<std::vector<ShopifyFulfillmentOrderRef>>::failure(response.error());
    std::vector<ShopifyFulfillmentOrderRef> output;
    const auto nodes = response.value().get("data").get("order").get("fulfillmentOrders").get("nodes");
    for (std::size_t index = 0; index < nodes.size(); ++index) {
        ShopifyFulfillmentOrderRef value{nodes.at(index).getString("id"), nodes.at(index).getString("status")};
        if (value.valid()) output.push_back(std::move(value));
    }
    return Result<std::vector<ShopifyFulfillmentOrderRef>>::success(std::move(output));
}

Result<void> ShopifyClient::createFulfillment(const std::string& fulfillment_order_gid,
                                               const ShipmentInfo& tracking,
                                               bool notify_customer) {
    if (fulfillment_order_gid.empty()) return Result<void>::failure("Fulfillment order ID is empty");
    Json variables = Json::object();
    Json fulfillment = Json::object();
    fulfillment.set("notifyCustomer", notify_customer);
    if (!tracking.tracking_number.empty()) {
        Json tracking_info = Json::object();
        tracking_info.set("number", tracking.tracking_number);
        tracking_info.set("company", tracking.carrier);
        fulfillment.set("trackingInfo", tracking_info);
    }
    Json groups = Json::array();
    Json group = Json::object();
    group.set("fulfillmentOrderId", fulfillment_order_gid);
    groups.push(group);
    fulfillment.set("lineItemsByFulfillmentOrder", groups);
    variables.set("fulfillment", fulfillment);
    auto response = graphql(
        R"graphql(mutation FulfillmentCreate($fulfillment:FulfillmentInput!){
          fulfillmentCreate(fulfillment:$fulfillment){fulfillment{id status} userErrors{field message}}
        })graphql",
        variables);
    if (!response) return Result<void>::failure(response.error());
    return ensureNoUserErrors(response.value().get("data").get("fulfillmentCreate"), "fulfillmentCreate");
}

Result<void> ShopifyClient::registerWebhook(const std::string& topic, const std::string& callback_url) {
    if (callback_url.rfind("https://", 0) != 0) {
        return Result<void>::failure("Shopify webhook callback must use HTTPS");
    }
    Json variables = Json::object();
    variables.set("topic", topic);
    Json subscription = Json::object();
    subscription.set("callbackUrl", callback_url);
    subscription.set("format", "JSON");
    variables.set("webhookSubscription", subscription);
    auto response = graphql(
        R"graphql(mutation WebhookSubscriptionCreate($topic:WebhookSubscriptionTopic!,$webhookSubscription:WebhookSubscriptionInput!){
          webhookSubscriptionCreate(topic:$topic,webhookSubscription:$webhookSubscription){
            webhookSubscription{id topic} userErrors{field message}
          }
        })graphql",
        variables);
    if (!response) return Result<void>::failure(response.error());
    return ensureNoUserErrors(
        response.value().get("data").get("webhookSubscriptionCreate"), "webhookSubscriptionCreate");
}

Result<std::vector<ShopifyWebhookSubscriptionRef>> ShopifyClient::webhookSubscriptions(int first) {
    Json variables = Json::object();
    variables.set("first", std::clamp(first, 1, 250));
    auto response = graphql(
        R"graphql(query WebhookSubscriptions($first:Int!){
          webhookSubscriptions(first:$first){nodes{
            id topic endpoint{__typename ... on WebhookHttpEndpoint{callbackUrl}}
          }}
        })graphql",
        variables);
    if (!response) return Result<std::vector<ShopifyWebhookSubscriptionRef>>::failure(response.error());
    const auto nodes = response.value().get("data").get("webhookSubscriptions").get("nodes");
    std::vector<ShopifyWebhookSubscriptionRef> output;
    for (std::size_t index = 0; index < nodes.size(); ++index) {
        ShopifyWebhookSubscriptionRef item;
        item.id = nodes.at(index).getString("id");
        item.topic = nodes.at(index).getString("topic");
        item.callback_url = nodes.at(index).get("endpoint").getString("callbackUrl");
        if (!item.id.empty()) output.push_back(std::move(item));
    }
    return Result<std::vector<ShopifyWebhookSubscriptionRef>>::success(std::move(output));
}

Result<void> ShopifyClient::deleteWebhook(const std::string& subscription_gid) {
    if (subscription_gid.empty()) return Result<void>::failure("Shopify webhook subscription ID is empty");
    Json variables = Json::object();
    variables.set("id", subscription_gid);
    auto response = graphql(
        R"graphql(mutation WebhookSubscriptionDelete($id:ID!){
          webhookSubscriptionDelete(id:$id){deletedWebhookSubscriptionId userErrors{field message}}
        })graphql",
        variables);
    if (!response) return Result<void>::failure(response.error());
    return ensureNoUserErrors(
        response.value().get("data").get("webhookSubscriptionDelete"), "webhookSubscriptionDelete");
}

Result<std::string> ShopifyClient::primaryPublicationId() {
    auto response = graphql(
        "query PrimaryPublication { publications(first:20){nodes{id name}} }", Json::object());
    if (!response) return Result<std::string>::failure(response.error());
    const auto nodes = response.value().get("data").get("publications").get("nodes");
    if (nodes.size() == 0) return Result<std::string>::failure("No Shopify publication is available");
    return Result<std::string>::success(nodes.at(0).getString("id"));
}

Result<void> ShopifyClient::publishProduct(const std::string& product_gid,
                                           const std::string& publication_gid) {
    Json variables = Json::object();
    variables.set("id", product_gid);
    Json publications = Json::array();
    Json publication = Json::object();
    publication.set("publicationId", publication_gid);
    publications.push(publication);
    variables.set("input", publications);
    auto response = graphql(
        R"graphql(mutation PublishProduct($id:ID!,$input:[PublicationInput!]!){
          publishablePublish(id:$id,input:$input){userErrors{field message}}
        })graphql",
        variables);
    if (!response) return Result<void>::failure(response.error());
    return ensureNoUserErrors(
        response.value().get("data").get("publishablePublish"), "publishablePublish");
}

ShopifyBulkOperationRef ShopifyClient::parseBulkOperation(const Json& node) {
    ShopifyBulkOperationRef value;
    value.id = node.getString("id");
    value.type = node.getString("type");
    value.status = node.getString("status");
    value.error_code = node.getString("errorCode");
    value.url = node.getString("url");
    value.partial_data_url = node.getString("partialDataUrl");
    value.object_count = static_cast<std::uint64_t>(std::max<std::int64_t>(0, node.getInt64("objectCount", 0)));
    value.root_object_count = static_cast<std::uint64_t>(
        std::max<std::int64_t>(0, node.getInt64("rootObjectCount", 0)));
    return value;
}

Result<ShopifyBulkOperationRef> ShopifyClient::runBulkQuery(const std::string& bulk_query) {
    if (bulk_query.empty()) return Result<ShopifyBulkOperationRef>::failure("Shopify bulk query is empty");
    Json variables = Json::object();
    variables.set("query", bulk_query);
    auto response = graphql(
        R"graphql(mutation BulkOperationRunQuery($query:String!){
          bulkOperationRunQuery(query:$query){
            bulkOperation{id type status errorCode objectCount rootObjectCount url partialDataUrl}
            userErrors{field message}
          }
        })graphql",
        variables);
    if (!response) return Result<ShopifyBulkOperationRef>::failure(response.error());
    const auto payload = response.value().get("data").get("bulkOperationRunQuery");
    auto valid = ensureNoUserErrors(payload, "bulkOperationRunQuery");
    if (!valid) return Result<ShopifyBulkOperationRef>::failure(valid.error());
    auto operation = parseBulkOperation(payload.get("bulkOperation"));
    if (!operation.valid()) return Result<ShopifyBulkOperationRef>::failure("Shopify returned an invalid bulk operation");
    return Result<ShopifyBulkOperationRef>::success(std::move(operation));
}

Result<std::vector<ShopifyBulkOperationRef>> ShopifyClient::bulkOperations(
    int first,
    const std::string& filter) {
    Json variables = Json::object();
    variables.set("first", std::clamp(first, 1, 100));
    if (!filter.empty()) variables.set("query", filter);
    auto response = graphql(
        R"graphql(query BulkOperations($first:Int!,$query:String){
          bulkOperations(first:$first,query:$query,sortKey:CREATED_AT,reverse:true){nodes{
            id type status errorCode objectCount rootObjectCount url partialDataUrl createdAt completedAt
          }}
        })graphql",
        variables);
    if (!response) return Result<std::vector<ShopifyBulkOperationRef>>::failure(response.error());
    const auto nodes = response.value().get("data").get("bulkOperations").get("nodes");
    std::vector<ShopifyBulkOperationRef> output;
    for (std::size_t index = 0; index < nodes.size(); ++index) {
        auto operation = parseBulkOperation(nodes.at(index));
        if (operation.valid()) output.push_back(std::move(operation));
    }
    return Result<std::vector<ShopifyBulkOperationRef>>::success(std::move(output));
}

Result<ShopifyBulkOperationRef> ShopifyClient::bulkOperation(const std::string& operation_gid) {
    if (operation_gid.empty()) return Result<ShopifyBulkOperationRef>::failure("Shopify bulk operation ID is empty");
    Json variables = Json::object();
    variables.set("id", operation_gid);
    auto response = graphql(
        R"graphql(query BulkOperation($id:ID!){
          bulkOperation(id:$id){id type status errorCode objectCount rootObjectCount url partialDataUrl}
        })graphql",
        variables);
    if (!response) return Result<ShopifyBulkOperationRef>::failure(response.error());
    auto operation = parseBulkOperation(response.value().get("data").get("bulkOperation"));
    if (!operation.valid()) return Result<ShopifyBulkOperationRef>::failure("Shopify bulk operation was not found");
    return Result<ShopifyBulkOperationRef>::success(std::move(operation));
}

Result<void> ShopifyClient::cancelBulkOperation(const std::string& operation_gid) {
    if (operation_gid.empty()) return Result<void>::failure("Shopify bulk operation ID is empty");
    Json variables = Json::object();
    variables.set("id", operation_gid);
    auto response = graphql(
        R"graphql(mutation BulkOperationCancel($id:ID!){
          bulkOperationCancel(id:$id){bulkOperation{id status} userErrors{field message}}
        })graphql",
        variables);
    if (!response) return Result<void>::failure(response.error());
    return ensureNoUserErrors(
        response.value().get("data").get("bulkOperationCancel"), "bulkOperationCancel");
}

} // namespace elit21
