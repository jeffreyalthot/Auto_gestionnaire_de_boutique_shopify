#pragma once

#include "elit21/core/Result.h"
#include "elit21/shopify/ShopifyClient.h"
#include "elit21/platform/BusinessComponent.h"

#include <stdexcept>
#include <string>
#include <vector>

namespace elit21::shopify::services {

class ShopifyBulkOperationService final : public platform::BusinessComponent {
public:
    ShopifyBulkOperationService();
    explicit ShopifyBulkOperationService(ShopifyClient& client);
    void bind(ShopifyClient& client) noexcept { client_ = &client; }
    [[nodiscard]] bool bound() const noexcept { return client_ != nullptr; }
    Result<ShopifyBulkOperationRef> runQuery(const std::string& query);
    Result<std::vector<ShopifyBulkOperationRef>> list(int first=20,const std::string& filter={});
    Result<ShopifyBulkOperationRef> get(const std::string& operation_gid);
    Result<void> cancel(const std::string& operation_gid);
    platform::OperationResult execute(const platform::OperationContext& context) override;
private:
    ShopifyClient& requireClient();
    ShopifyClient* client_{nullptr};
};

} // namespace elit21::shopify::services
