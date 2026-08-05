#pragma once

#include "elit21/core/Result.h"
#include "elit21/json/Json.h"
#include "elit21/shopify/ShopifyModels.h"
#include "elit21/shopify/ShopifyMutationCoordinator.h"

#include <string>

namespace elit21 {
class Database;
class ShopifyClient;
}

namespace elit21::shopify {

struct ShopifyProductSetPlan {
    std::string external_product_id;
    std::string business_key;
    bool synchronous{false};
    Json variables{Json::object()};

    [[nodiscard]] Json toJson() const;
};

struct ShopifyProductSetResult {
    std::string product_gid;
    std::string operation_gid;
    std::string operation_status;
    std::string idempotency_key;
    bool asynchronous{false};
    bool replayed{false};

    [[nodiscard]] Json toJson() const;
};

class ShopifyProductSetSyncService {
public:
    ShopifyProductSetSyncService(ShopifyClient& client, Database& database);

    [[nodiscard]] static Result<ShopifyProductSetPlan> plan(
        const ShopifyProductCreate& product,
        bool synchronous = false);

    Result<ShopifyProductSetResult> execute(
        const ShopifyProductCreate& product,
        bool synchronous = false);
    Result<ShopifyProductSetResult> operationStatus(const std::string& operation_gid);
    Result<ShopifyProductSetResult> waitForCompletion(
        const std::string& operation_gid,
        int maximum_polls = 60,
        int poll_interval_ms = 1000);

    [[nodiscard]] static const std::string& mutationDocument();
    [[nodiscard]] static const std::string& operationQueryDocument();

private:
    ShopifyClient& client_;
    Database& database_;
    ShopifyMutationCoordinator coordinator_;
};

} // namespace elit21::shopify
