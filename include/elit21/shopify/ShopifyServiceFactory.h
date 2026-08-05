#pragma once

#include "elit21/config/Config.h"
#include "elit21/core/Result.h"
#include "elit21/net/HttpClient.h"
#include "elit21/platform/BusinessComponent.h"
#include "elit21/shopify/ShopifyClient.h"

#include <memory>

namespace elit21::shopify {

class ShopifyServiceFactory final : public platform::BusinessComponent {
public:
    ShopifyServiceFactory();
    ShopifyServiceFactory(::elit21::ShopifyConfig config, HttpClient& http);
    void bind(::elit21::ShopifyConfig config, HttpClient& http);
    [[nodiscard]] bool ready() const noexcept;
    Result<std::unique_ptr<ShopifyClient>> createClient() const;
    Result<void> validate() const;
    platform::OperationResult execute(const platform::OperationContext& context) override;
private:
    ::elit21::ShopifyConfig config_;
    HttpClient* http_{nullptr};
};

} // namespace elit21::shopify
