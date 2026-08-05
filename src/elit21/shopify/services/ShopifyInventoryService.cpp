#include "elit21/shopify/services/ShopifyInventoryService.h"

namespace elit21::shopify::services {
namespace {

Json inventoryVariables(const std::string& location_gid,
                        const std::vector<ShopifyInventoryQuantityUpdate>& updates,
                        const std::string& reference_document_uri,
                        bool allow_unchecked) {
    Json output = Json::object();
    output.set("location_id", location_gid);
    output.set("reference_document_uri", reference_document_uri);
    output.set("allow_unchecked", allow_unchecked);
    Json quantities = Json::array();
    for (const auto& update : updates) {
        Json value = Json::object();
        value.set("inventory_item_id", update.inventory_item_id);
        value.set("quantity", update.quantity);
        value.set("compare_enabled", update.compare_quantity.has_value());
        if (update.compare_quantity.has_value()) {
            value.set("compare_quantity", *update.compare_quantity);
        }
        quantities.push(value);
    }
    output.set("quantities", quantities);
    return output;
}

} // namespace

ShopifyInventoryService::ShopifyInventoryService()
    : platform::BusinessComponent(
          "ShopifyInventoryService",
          "Durable idempotent Shopify inventory service with compare-and-set support",
          platform::BusinessComponentSpec{
              "shopify", "set_inventory_quantities", {}, true, true, 4U * 1024U * 1024U}) {}

ShopifyInventoryService::ShopifyInventoryService(ShopifyClient& client)
    : ShopifyInventoryService() {
    client_ = &client;
}

ShopifyInventoryService::ShopifyInventoryService(ShopifyClient& client, Database& database)
    : ShopifyInventoryService(client) {
    database_ = &database;
}

ShopifyClient& ShopifyInventoryService::requireClient() {
    if (!client_) throw std::logic_error("ShopifyInventoryService is not bound to a ShopifyClient");
    return *client_;
}

Result<void> ShopifyInventoryService::setQuantity(const std::string& inventory_item_gid,
                                                   const std::string& location_gid,
                                                   int quantity) {
    return requireClient().updateInventory(inventory_item_gid, location_gid, quantity);
}

Result<void> ShopifyInventoryService::setQuantities(
    const std::string& location_gid,
    const std::vector<ShopifyInventoryQuantityUpdate>& updates) {
    return requireClient().updateInventories(location_gid, updates);
}

Result<void> ShopifyInventoryService::setQuantitiesSafely(
    const std::string& location_gid,
    const std::vector<ShopifyInventoryQuantityUpdate>& updates,
    const std::string& business_key,
    const std::string& reference_document_uri,
    bool allow_unchecked) {
    if (business_key.empty()) {
        return Result<void>::failure("Shopify inventory business key is empty");
    }
    if (database_ == nullptr) {
        return Result<void>::failure(
            "Durable Shopify inventory writes require a bound Database");
    }

    const auto variables = inventoryVariables(
        location_gid, updates, reference_document_uri, allow_unchecked);
    ShopifyIdempotencyStore store(*database_);
    auto reservation = store.reserve(
        "inventorySetQuantities", variables, business_key, 24);
    if (!reservation) return Result<void>::failure(reservation.error());
    if (reservation.value().completed()) return Result<void>::success();
    if (reservation.value().concurrent()) {
        return Result<void>::failure(
            "A Shopify inventory request with this business key is already in progress");
    }

    auto in_progress = store.markInProgress(reservation.value());
    if (!in_progress) return in_progress;
    auto updated = requireClient().updateInventoriesIdempotent(
        location_gid, updates, reservation.value().key,
        reference_document_uri, allow_unchecked);
    if (!updated) {
        store.fail(reservation.value(), updated.error());
        return updated;
    }
    Json response = Json::object();
    response.set("status", "completed");
    response.set("quantity_count", static_cast<std::int64_t>(updates.size()));
    response.set("location_id", location_gid);
    return store.complete(reservation.value(), response);
}

platform::OperationResult ShopifyInventoryService::execute(
    const platform::OperationContext& context) {
    auto result = executeBusiness(context);
    result.metrics["client_bound"] = bound() ? 1.0 : 0.0;
    result.metrics["durable_idempotency"] = durableIdempotencyEnabled() ? 1.0 : 0.0;
    result.attributes["inventory_write_mode"] = "idempotent_compare_and_set";
    return result;
}

} // namespace elit21::shopify::services
