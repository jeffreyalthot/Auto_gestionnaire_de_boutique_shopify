#pragma once

#include "elit21/core/Result.h"
#include "elit21/platform/BusinessComponent.h"
#include "elit21/shopify/ShopifyClient.h"
#include "elit21/shopify/ShopifyIdempotencyStore.h"
#include "elit21/storage/Database.h"

#include <stdexcept>
#include <string>
#include <vector>

namespace elit21::shopify::services {

class ShopifyInventoryService final : public platform::BusinessComponent {
public:
    ShopifyInventoryService();
    explicit ShopifyInventoryService(ShopifyClient& client);
    ShopifyInventoryService(ShopifyClient& client, Database& database);

    void bind(ShopifyClient& client) noexcept { client_ = &client; }
    void bind(Database& database) noexcept { database_ = &database; }
    [[nodiscard]] bool bound() const noexcept { return client_ != nullptr; }
    [[nodiscard]] bool durableIdempotencyEnabled() const noexcept { return database_ != nullptr; }

    Result<void> setQuantity(const std::string& inventory_item_gid,
                             const std::string& location_gid,
                             int quantity);
    Result<void> setQuantities(
        const std::string& location_gid,
        const std::vector<ShopifyInventoryQuantityUpdate>& updates);
    Result<void> setQuantitiesSafely(
        const std::string& location_gid,
        const std::vector<ShopifyInventoryQuantityUpdate>& updates,
        const std::string& business_key,
        const std::string& reference_document_uri,
        bool allow_unchecked = false);

    platform::OperationResult execute(const platform::OperationContext& context) override;

private:
    ShopifyClient& requireClient();
    ShopifyClient* client_{nullptr};
    Database* database_{nullptr};
};

} // namespace elit21::shopify::services
