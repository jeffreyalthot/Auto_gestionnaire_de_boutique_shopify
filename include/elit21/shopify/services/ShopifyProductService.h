#pragma once

#include "elit21/core/Result.h"
#include "elit21/shopify/ShopifyClient.h"
#include "elit21/platform/BusinessComponent.h"

#include <stdexcept>
#include <string>
#include <vector>

namespace elit21::shopify::services {

class ShopifyProductService final : public platform::BusinessComponent {
public:
    ShopifyProductService();
    explicit ShopifyProductService(ShopifyClient& client);
    void bind(ShopifyClient& client) noexcept { client_ = &client; }
    [[nodiscard]] bool bound() const noexcept { return client_ != nullptr; }
    Result<std::string> create(const ShopifyProductCreate& product);
    Result<std::vector<ShopifyVariantRef>> variants(const std::string& product_gid);
    platform::OperationResult execute(const platform::OperationContext& context) override;
private:
    ShopifyClient& requireClient();
    ShopifyClient* client_{nullptr};
};

} // namespace elit21::shopify::services
