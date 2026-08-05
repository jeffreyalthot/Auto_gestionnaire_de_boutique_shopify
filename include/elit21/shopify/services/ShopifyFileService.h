#pragma once

#include "elit21/core/Result.h"
#include "elit21/json/Json.h"
#include "elit21/platform/BusinessComponent.h"
#include "elit21/shopify/ShopifyClient.h"

#include <stdexcept>
#include <string>

namespace elit21::shopify::services {

class ShopifyFileService final : public platform::BusinessComponent {
public:
    ShopifyFileService();
    explicit ShopifyFileService(ShopifyClient& client);
    void bind(ShopifyClient& client) noexcept { client_ = &client; }
    [[nodiscard]] bool bound() const noexcept { return client_ != nullptr; }
    Result<Json> executeDocument(const std::string& query, const Json& variables = Json::object(),
                                 const std::string& response_field = {});
    Result<Json> filesPage(int first = 100, const std::string& after = {},
                         const std::string& search_or_id = {});
    platform::OperationResult execute(const platform::OperationContext& context) override;
private:
    ShopifyClient& requireClient();
    ShopifyClient* client_{nullptr};
};

} // namespace elit21::shopify::services
