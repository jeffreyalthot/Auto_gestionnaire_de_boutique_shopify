#pragma once

#include "elit21/core/Result.h"
#include "elit21/platform/BusinessComponent.h"
#include "elit21/shopify/ShopifyClient.h"

#include <chrono>
#include <functional>
#include <string>

namespace elit21::shopify::bulk {

class BulkOperationMonitor final : public platform::BusinessComponent {
public:
    using ProgressCallback = std::function<void(const ShopifyBulkOperationRef&)>;

    BulkOperationMonitor();
    explicit BulkOperationMonitor(ShopifyClient& client);
    void bind(ShopifyClient& client) noexcept { client_ = &client; }
    Result<ShopifyBulkOperationRef> wait(const std::string& operation_gid,
                                         std::chrono::seconds timeout = std::chrono::minutes(30),
                                         std::chrono::milliseconds interval = std::chrono::seconds(2),
                                         const ProgressCallback& callback = {});
    Result<void> cancel(const std::string& operation_gid);
    platform::OperationResult execute(const platform::OperationContext& context) override;
private:
    ShopifyClient* client_{nullptr};
};

} // namespace elit21::shopify::bulk
