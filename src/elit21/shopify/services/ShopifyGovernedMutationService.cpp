#include "elit21/shopify/services/ShopifyGovernedMutationService.h"

#include "elit21/json/Json.h"
#include "elit21/shopify/ShopifyClient.h"
#include "elit21/storage/Database.h"

namespace elit21::shopify::services {

ShopifyGovernedMutationService::ShopifyGovernedMutationService()
    : platform::BusinessComponent(
          "ShopifyGovernedMutationService",
          "Persistent idempotent Shopify mutation coordinator",
          platform::BusinessComponentSpec{
              "shopify", "execute_governed_mutation", {}, true, true, 4U * 1024U * 1024U}) {}

ShopifyGovernedMutationService::ShopifyGovernedMutationService(
    ShopifyClient& client,
    Database& database)
    : ShopifyGovernedMutationService() {
    bind(client, database);
}

void ShopifyGovernedMutationService::bind(ShopifyClient& client, Database& database) noexcept {
    client_ = &client;
    database_ = &database;
}

bool ShopifyGovernedMutationService::bound() const noexcept {
    return client_ != nullptr && database_ != nullptr && database_->isOpen();
}

Result<ShopifyMutationReceipt> ShopifyGovernedMutationService::executeMutation(
    const std::string& operation_name,
    const std::string& query,
    Json variables,
    const std::string& business_key,
    int maximum_attempts,
    bool inject_idempotency_key) {
    if (!bound()) {
        return Result<ShopifyMutationReceipt>::failure(
            "ShopifyGovernedMutationService requires a ShopifyClient and an open Database");
    }
    ShopifyMutationCoordinator coordinator(*client_, *database_);
    return coordinator.execute(operation_name, query, std::move(variables), business_key,
                               maximum_attempts, inject_idempotency_key);
}

platform::OperationResult ShopifyGovernedMutationService::execute(
    const platform::OperationContext& context) {
    auto result = executeBusiness(context);
    result.metrics["client_bound"] = client_ != nullptr ? 1.0 : 0.0;
    result.metrics["database_bound"] = database_ != nullptr && database_->isOpen() ? 1.0 : 0.0;
    result.attributes["mutation_mode"] = "persistent_idempotent";
    return result;
}

} // namespace elit21::shopify::services
