#include "elit21/shopify/ShopifyProductSetSyncService.h"

#include "elit21/shopify/ShopifyClient.h"
#include "elit21/shopify/ShopifyGlobalId.h"
#include "elit21/shopify/graphql/ShopifyUserErrorParser.h"
#include "elit21/shopify/ShopifyMoney.h"
#include "elit21/storage/Database.h"

#include <algorithm>
#include <chrono>
#include <set>
#include <thread>

namespace elit21::shopify {
namespace {

const std::string kProductSetMutation = R"graphql(
mutation ProductSetExternalSync(
  $identifier: ProductSetIdentifiers,
  $input: ProductSetInput!,
  $synchronous: Boolean!
) {
  productSet(identifier: $identifier, input: $input, synchronous: $synchronous) {
    product {
      id
      handle
      title
      status
      variants(first: 250) {
        nodes { id sku inventoryItem { id } }
      }
    }
    productSetOperation {
      id
      status
      product { id }
      userErrors { code field message }
    }
    userErrors { code field message }
  }
}
)graphql";

const std::string kProductOperationQuery = R"graphql(
query ProductSetOperationStatus($id: ID!) {
  productOperation(id: $id) {
    ... on ProductSetOperation {
      id
      status
      product { id }
      userErrors { code field message }
    }
  }
}
)graphql";

Json sourceMetafield(const std::string& value) {
    Json field = Json::object();
    field.set("namespace", "elit21");
    field.set("key", "aliexpress_product_id");
    field.set("type", "single_line_text_field");
    field.set("value", value);
    return field;
}

} // namespace

Json ShopifyProductSetPlan::toJson() const {
    Json output = Json::object();
    output.set("external_product_id", external_product_id);
    output.set("business_key", business_key);
    output.set("synchronous", synchronous);
    output.set("variables", variables);
    return output;
}

Json ShopifyProductSetResult::toJson() const {
    Json output = Json::object();
    output.set("product_gid", product_gid);
    output.set("operation_gid", operation_gid);
    output.set("operation_status", operation_status);
    output.set("idempotency_key", idempotency_key);
    output.set("asynchronous", asynchronous);
    output.set("replayed", replayed);
    return output;
}

ShopifyProductSetSyncService::ShopifyProductSetSyncService(
    ShopifyClient& client,
    Database& database)
    : client_(client), database_(database), coordinator_(client, database) {}

const std::string& ShopifyProductSetSyncService::mutationDocument() { return kProductSetMutation; }
const std::string& ShopifyProductSetSyncService::operationQueryDocument() { return kProductOperationQuery; }

Result<ShopifyProductSetPlan> ShopifyProductSetSyncService::plan(
    const ShopifyProductCreate& product,
    bool synchronous) {
    const auto validation = ShopifyModelValidator::validateProduct(product, 100.0);
    if (!validation.valid) {
        return Result<ShopifyProductSetPlan>::failure(
            "Shopify productSet input failed validation: " + validation.toJson().dump());
    }
    if (product.variants.size() > 2048) {
        return Result<ShopifyProductSetPlan>::failure(
            "Shopify productSet supports at most 2048 variants per product");
    }

    Json identifier = Json::object();
    Json custom_id = Json::object();
    custom_id.set("namespace", "elit21");
    custom_id.set("key", "aliexpress_product_id");
    custom_id.set("value", product.source_product_id);
    identifier.set("customId", custom_id);

    Json input = Json::object();
    input.set("title", product.title);
    input.set("descriptionHtml", product.description_html);
    input.set("vendor", product.vendor);
    input.set("productType", product.product_type);
    input.set("status", product.publish ? "ACTIVE" : "DRAFT");

    Json metafields = Json::array();
    metafields.push(sourceMetafield(product.source_product_id));
    input.set("metafields", metafields);

    std::vector<ShopifyProductVariantCreate> variants = product.variants;
    if (variants.empty()) {
        ShopifyProductVariantCreate variant;
        variant.option_name = "Title";
        variant.option_value = product.source_sku_id.empty() ? "Default" : product.source_sku_id;
        variant.sku = product.sku;
        variant.source_product_id = product.source_product_id;
        variant.source_sku_id = product.source_sku_id;
        variant.price_cad = product.price_cad;
        variant.supplier_cost_cad = product.supplier_cost_cad;
        variant.inventory = product.inventory;
        variants.push_back(std::move(variant));
    }

    std::set<std::string> option_names;
    for (const auto& variant : variants) option_names.insert(variant.option_name.empty() ? "Title" : variant.option_name);
    Json product_options = Json::array();
    for (const auto& option_name : option_names) {
        Json option = Json::object();
        option.set("name", option_name);
        Json values = Json::array();
        std::set<std::string> unique_values;
        for (const auto& variant : variants) {
            const auto current_name = variant.option_name.empty() ? "Title" : variant.option_name;
            if (current_name != option_name || !unique_values.insert(variant.option_value).second) continue;
            Json value = Json::object();
            value.set("name", variant.option_value);
            values.push(value);
        }
        option.set("values", values);
        product_options.push(option);
    }
    input.set("productOptions", product_options);

    Json variant_inputs = Json::array();
    for (const auto& variant : variants) {
        Json item = Json::object();
        item.set("sku", variant.sku);
        item.set("price", ShopifyMoney::fromDouble(variant.price_cad).value().toDecimal());
        item.set("taxable", variant.taxable);
        item.set("inventoryPolicy", "DENY");

        Json inventory_item = Json::object();
        inventory_item.set("sku", variant.sku);
        inventory_item.set("tracked", true);
        inventory_item.set("requiresShipping", true);
        inventory_item.set("cost", ShopifyMoney::fromDouble(variant.supplier_cost_cad).value().toDecimal());
        item.set("inventoryItem", inventory_item);

        Json option_values = Json::array();
        Json option_value = Json::object();
        option_value.set("optionName", variant.option_name.empty() ? "Title" : variant.option_name);
        option_value.set("name", variant.option_value);
        option_values.push(option_value);
        item.set("optionValues", option_values);

        Json variant_metafields = Json::array();
        Json sku_field = Json::object();
        sku_field.set("namespace", "elit21");
        sku_field.set("key", "aliexpress_sku_id");
        sku_field.set("type", "single_line_text_field");
        sku_field.set("value", variant.source_sku_id);
        variant_metafields.push(sku_field);
        item.set("metafields", variant_metafields);
        variant_inputs.push(item);
    }
    input.set("variants", variant_inputs);

    Json variables = Json::object();
    variables.set("identifier", identifier);
    variables.set("input", input);
    variables.set("synchronous", synchronous);

    ShopifyProductSetPlan output;
    output.external_product_id = product.source_product_id;
    output.business_key = "product-set:" + product.source_product_id;
    output.synchronous = synchronous;
    output.variables = std::move(variables);
    return Result<ShopifyProductSetPlan>::success(std::move(output));
}

Result<ShopifyProductSetResult> ShopifyProductSetSyncService::execute(
    const ShopifyProductCreate& product,
    bool synchronous) {
    auto planned = plan(product, synchronous);
    if (!planned) return Result<ShopifyProductSetResult>::failure(planned.error());

    auto receipt = coordinator_.execute(
        "productSet", kProductSetMutation, planned.value().variables,
        planned.value().business_key, 4, false);
    if (!receipt) {
        database_.recordShopifyCatalogSync(
            product.source_product_id, synchronous ? "synchronous" : "asynchronous",
            "failed", {}, {}, Json::object().dump(), receipt.error());
        return Result<ShopifyProductSetResult>::failure(receipt.error());
    }

    const auto payload = receipt.value().response.get("data").get("productSet");
    ShopifyProductSetResult output;
    output.product_gid = payload.get("product").getString("id");
    const auto operation = payload.get("productSetOperation");
    output.operation_gid = operation.getString("id");
    output.operation_status = operation.getString("status", synchronous ? "COMPLETE" : "CREATED");
    if (output.product_gid.empty()) output.product_gid = operation.get("product").getString("id");
    output.idempotency_key = receipt.value().idempotency_key;
    output.asynchronous = !synchronous;
    output.replayed = receipt.value().replayed;

    auto stored = database_.recordShopifyCatalogSync(
        product.source_product_id, synchronous ? "synchronous" : "asynchronous",
        output.operation_status.empty() ? "submitted" : output.operation_status,
        output.product_gid, output.operation_gid, output.toJson().dump(), {});
    if (!stored) return Result<ShopifyProductSetResult>::failure(stored.error());
    return Result<ShopifyProductSetResult>::success(std::move(output));
}


Result<ShopifyProductSetResult> ShopifyProductSetSyncService::operationStatus(
    const std::string& operation_gid) {
    auto gid = ShopifyGlobalId::parse(operation_gid);
    if (!gid || !gid.value().isType("ProductSetOperation")) {
        return Result<ShopifyProductSetResult>::failure(
            "Product operation ID must be a ProductSetOperation GID");
    }
    Json variables = Json::object();
    variables.set("id", operation_gid);
    auto response = client_.graphql(kProductOperationQuery, variables, 4);
    if (!response) return Result<ShopifyProductSetResult>::failure(response.error());
    const auto node = response.value().get("data").get("productOperation");
    if (node.isNull()) {
        return Result<ShopifyProductSetResult>::failure("Shopify product operation was not found");
    }
    const auto errors = graphql::ShopifyUserErrorParser::findRecursively(node);
    if (!errors.empty()) {
        return Result<ShopifyProductSetResult>::failure(
            graphql::ShopifyUserErrorParser::join(errors));
    }
    ShopifyProductSetResult output;
    output.operation_gid = node.getString("id", operation_gid);
    output.operation_status = node.getString("status");
    output.product_gid = node.get("product").getString("id");
    output.asynchronous = true;
    return Result<ShopifyProductSetResult>::success(std::move(output));
}

Result<ShopifyProductSetResult> ShopifyProductSetSyncService::waitForCompletion(
    const std::string& operation_gid,
    int maximum_polls,
    int poll_interval_ms) {
    maximum_polls = std::clamp(maximum_polls, 1, 300);
    poll_interval_ms = std::clamp(poll_interval_ms, 50, 10000);
    ShopifyProductSetResult last;
    for (int poll = 0; poll < maximum_polls; ++poll) {
        auto status = operationStatus(operation_gid);
        if (!status) return status;
        last = status.value();
        if (last.operation_status == "COMPLETE") {
            return Result<ShopifyProductSetResult>::success(std::move(last));
        }
        if (poll + 1 < maximum_polls) {
            std::this_thread::sleep_for(std::chrono::milliseconds(poll_interval_ms));
        }
    }
    return Result<ShopifyProductSetResult>::failure(
        "Shopify productSet operation did not complete within the polling budget");
}

} // namespace elit21::shopify
