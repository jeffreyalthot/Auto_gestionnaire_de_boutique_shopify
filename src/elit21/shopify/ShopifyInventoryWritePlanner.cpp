#include "elit21/shopify/ShopifyInventoryWritePlanner.h"

#include "elit21/shopify/ShopifyGlobalId.h"
#include "elit21/shopify/ShopifyIdempotencyKey.h"

#include <algorithm>

namespace elit21::shopify {
namespace {

const std::string kInventoryMutation = R"graphql(
mutation InventorySetQuantities(
  $input: InventorySetQuantitiesInput!,
  $idempotencyKey: String!
) {
  inventorySetQuantities(input: $input) @idempotent(key: $idempotencyKey) {
    inventoryAdjustmentGroup {
      createdAt
      reason
      referenceDocumentUri
      changes {
        name
        delta
        quantityAfterChange
      }
    }
    userErrors {
      code
      field
      message
    }
  }
}
)graphql";

bool validReferenceUri(const std::string& value) {
    if (value.empty()) return true;
    const auto separator = value.find("://");
    return separator != std::string::npos && separator > 0 && separator + 3 < value.size();
}

} // namespace

Json ShopifyInventoryWriteBatch::toJson() const {
    Json output = Json::object();
    output.set("offset", static_cast<std::int64_t>(offset));
    output.set("count", static_cast<std::int64_t>(count));
    output.set("idempotency_key", idempotency_key);
    output.set("compare_and_set", compare_and_set);
    output.set("variables", variables);
    return output;
}

const std::string& ShopifyInventoryWritePlanner::mutationDocument() {
    return kInventoryMutation;
}

Result<void> ShopifyInventoryWritePlanner::auditMutationDocument(const std::string& document) {
    if (document.find("inventorySetQuantities") == std::string::npos) {
        return Result<void>::failure("inventorySetQuantities mutation is missing");
    }
    if (document.find("@idempotent(key:") == std::string::npos) {
        return Result<void>::failure("Shopify 2026-07 inventory mutation requires @idempotent");
    }
    if (document.find("compareQuantity") != std::string::npos ||
        document.find("ignoreCompareQuantity") != std::string::npos) {
        return Result<void>::failure("Legacy inventory compare fields are forbidden in 2026-07");
    }
    if (document.find("userErrors") == std::string::npos) {
        return Result<void>::failure("inventorySetQuantities must request userErrors");
    }
    return Result<void>::success();
}

Result<std::vector<ShopifyInventoryWriteBatch>> ShopifyInventoryWritePlanner::build(
    const std::string& location_gid,
    const std::vector<ShopifyInventoryQuantityUpdate>& updates,
    const std::string& base_idempotency_key,
    const std::string& reference_document_uri,
    bool allow_unchecked) {
    auto location = ShopifyGlobalId::parse(location_gid);
    if (!location || !location.value().isType("Location")) {
        return Result<std::vector<ShopifyInventoryWriteBatch>>::failure(
            "Shopify inventory location must be a Location GID");
    }
    if (updates.empty()) {
        return Result<std::vector<ShopifyInventoryWriteBatch>>::success({});
    }
    if (!validReferenceUri(reference_document_uri)) {
        return Result<std::vector<ShopifyInventoryWriteBatch>>::failure(
            "Inventory referenceDocumentUri must contain a URI scheme");
    }
    auto key_validation = ShopifyIdempotencyKey::validate(base_idempotency_key);
    if (!key_validation) {
        return Result<std::vector<ShopifyInventoryWriteBatch>>::failure(key_validation.error());
    }
    auto document_validation = auditMutationDocument(kInventoryMutation);
    if (!document_validation) {
        return Result<std::vector<ShopifyInventoryWriteBatch>>::failure(document_validation.error());
    }

    std::vector<ShopifyInventoryWriteBatch> batches;
    for (std::size_t offset = 0; offset < updates.size(); offset += maximum_input_array_size) {
        const auto end = std::min(updates.size(), offset + maximum_input_array_size);
        Json input = Json::object();
        input.set("reason", "correction");
        input.set("name", "available");
        if (!reference_document_uri.empty()) input.set("referenceDocumentUri", reference_document_uri);

        Json quantities = Json::array();
        bool checked = true;
        for (std::size_t index = offset; index < end; ++index) {
            const auto& update = updates[index];
            if (!update.valid()) {
                return Result<std::vector<ShopifyInventoryWriteBatch>>::failure(
                    "Invalid Shopify inventory update at index " + std::to_string(index));
            }
            auto item = ShopifyGlobalId::parse(update.inventory_item_id);
            if (!item || !item.value().isType("InventoryItem")) {
                return Result<std::vector<ShopifyInventoryWriteBatch>>::failure(
                    "Inventory item must be an InventoryItem GID at index " + std::to_string(index));
            }
            Json quantity = Json::object();
            quantity.set("inventoryItemId", update.inventory_item_id);
            quantity.set("locationId", location_gid);
            quantity.set("quantity", update.quantity);
            if (update.compare_quantity.has_value()) {
                quantity.set("changeFromQuantity", *update.compare_quantity);
            } else {
                if (!allow_unchecked) {
                    return Result<std::vector<ShopifyInventoryWriteBatch>>::failure(
                        "changeFromQuantity is required for safe inventory writes at index " +
                        std::to_string(index));
                }
                quantity.set("changeFromQuantity", Json());
                checked = false;
            }
            quantities.push(quantity);
        }
        input.set("quantities", quantities);

        Json key_material = Json::object();
        key_material.set("input", input);
        key_material.set("batch_offset", static_cast<std::int64_t>(offset));
        const auto batch_key = ShopifyIdempotencyKey::deterministic(
            "inventorySetQuantities", key_material,
            base_idempotency_key + ":" + std::to_string(offset));

        Json variables = Json::object();
        variables.set("input", input);
        variables.set("idempotencyKey", batch_key);

        ShopifyInventoryWriteBatch batch;
        batch.offset = offset;
        batch.count = end - offset;
        batch.idempotency_key = batch_key;
        batch.variables = std::move(variables);
        batch.compare_and_set = checked;
        batches.push_back(std::move(batch));
    }
    return Result<std::vector<ShopifyInventoryWriteBatch>>::success(std::move(batches));
}

} // namespace elit21::shopify
