#pragma once

#include "elit21/platform/BusinessComponent.h"
#include "elit21/shopify/ShopifyMutationCoordinator.h"

#include <string>

namespace elit21 {
class Database;
class ShopifyClient;
}

namespace elit21::shopify::services {

class ShopifyGovernedMutationService final : public platform::BusinessComponent {
public:
    ShopifyGovernedMutationService();
    ShopifyGovernedMutationService(ShopifyClient& client, Database& database);

    void bind(ShopifyClient& client, Database& database) noexcept;
    [[nodiscard]] bool bound() const noexcept;

    Result<ShopifyMutationReceipt> executeMutation(const std::string& operation_name,
                                                    const std::string& query,
                                                    Json variables,
                                                    const std::string& business_key,
                                                    int maximum_attempts = 4,
                                                    bool inject_idempotency_key = true);

    platform::OperationResult execute(const platform::OperationContext& context) override;

private:
    ShopifyClient* client_{nullptr};
    Database* database_{nullptr};
};

} // namespace elit21::shopify::services
