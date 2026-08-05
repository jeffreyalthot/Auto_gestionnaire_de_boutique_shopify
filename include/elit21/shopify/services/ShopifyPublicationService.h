#pragma once

#include "elit21/core/Result.h"
#include "elit21/shopify/ShopifyClient.h"
#include "elit21/platform/BusinessComponent.h"

#include <stdexcept>
#include <string>
#include <vector>

namespace elit21::shopify::services {

class ShopifyPublicationService final : public platform::BusinessComponent {
public:
    ShopifyPublicationService();
    explicit ShopifyPublicationService(ShopifyClient& client);
    void bind(ShopifyClient& client) noexcept { client_ = &client; }
    [[nodiscard]] bool bound() const noexcept { return client_ != nullptr; }
    Result<std::string> primaryPublication();
    Result<void> publish(const std::string& product_gid,const std::string& publication_gid);
    platform::OperationResult execute(const platform::OperationContext& context) override;
private:
    ShopifyClient& requireClient();
    ShopifyClient* client_{nullptr};
};

} // namespace elit21::shopify::services
