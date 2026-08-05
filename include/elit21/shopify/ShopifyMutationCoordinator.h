#pragma once

#include "elit21/core/Result.h"
#include "elit21/json/Json.h"
#include "elit21/shopify/ShopifyIdempotencyStore.h"

#include <string>

namespace elit21 {
class Database;
class ShopifyClient;
}

namespace elit21::shopify {

struct ShopifyMutationReceipt {
    std::string operation_name;
    std::string business_key;
    std::string idempotency_key;
    Json response{Json::object()};
    bool replayed{false};

    [[nodiscard]] Json toJson() const;
};

class ShopifyMutationCoordinator {
public:
    ShopifyMutationCoordinator(ShopifyClient& client, Database& database);

    Result<ShopifyMutationReceipt> execute(const std::string& operation_name,
                                           const std::string& query,
                                           Json variables,
                                           const std::string& business_key,
                                           int maximum_attempts = 4,
                                           bool inject_idempotency_key = true);

private:
    Result<ShopifyMutationReceipt> replay(const ShopifyIdempotencyHandle& handle,
                                          const std::string& business_key) const;

    ShopifyClient& client_;
    Database& database_;
    ShopifyIdempotencyStore idempotency_;
};

} // namespace elit21::shopify
