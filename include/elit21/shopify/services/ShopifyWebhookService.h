#pragma once

#include "elit21/core/Result.h"
#include "elit21/shopify/ShopifyClient.h"
#include "elit21/platform/BusinessComponent.h"

#include <stdexcept>
#include <string>
#include <vector>

namespace elit21::shopify::services {

class ShopifyWebhookService final : public platform::BusinessComponent {
public:
    ShopifyWebhookService();
    explicit ShopifyWebhookService(ShopifyClient& client);
    void bind(ShopifyClient& client) noexcept { client_ = &client; }
    [[nodiscard]] bool bound() const noexcept { return client_ != nullptr; }
    Result<void> registerTopic(const std::string& topic,const std::string& callback_url);
    Result<std::vector<ShopifyWebhookSubscriptionRef>> subscriptions(int first=100);
    Result<void> remove(const std::string& subscription_gid);
    platform::OperationResult execute(const platform::OperationContext& context) override;
private:
    ShopifyClient& requireClient();
    ShopifyClient* client_{nullptr};
};

} // namespace elit21::shopify::services
