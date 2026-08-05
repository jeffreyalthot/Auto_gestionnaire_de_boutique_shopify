#pragma once

#include "elit21/core/Result.h"
#include "elit21/shopify/ShopifyClient.h"
#include "elit21/platform/BusinessComponent.h"

#include <stdexcept>
#include <string>
#include <vector>

namespace elit21::shopify::services {

class ShopifyVariantService final : public platform::BusinessComponent {
public:
    ShopifyVariantService();
    explicit ShopifyVariantService(ShopifyClient& client);
    void bind(ShopifyClient& client) noexcept { client_ = &client; }
    [[nodiscard]] bool bound() const noexcept { return client_ != nullptr; }
    Result<std::vector<ShopifyVariantRef>> create(const std::string& product_gid,const std::vector<ShopifyProductVariantCreate>& variants,bool remove_standalone=true);
    Result<void> updatePrices(const std::string& product_gid,const std::vector<ShopifyVariantPriceUpdate>& updates);
    platform::OperationResult execute(const platform::OperationContext& context) override;
private:
    ShopifyClient& requireClient();
    ShopifyClient* client_{nullptr};
};

} // namespace elit21::shopify::services
