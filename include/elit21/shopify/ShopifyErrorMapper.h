#pragma once

#include "elit21/net/HttpClient.h"
#include "elit21/platform/BusinessComponent.h"
#include "elit21/shopify/ShopifyError.h"
#include "elit21/shopify/graphql/GraphqlResponse.h"

#include <string>

namespace elit21::shopify {

class ShopifyErrorMapper final : public platform::BusinessComponent {
public:
    ShopifyErrorMapper();
    [[nodiscard]] static ShopifyErrorInfo fromHttp(long status,
                                                    const std::string& body,
                                                    int retry_after_seconds = 0);
    [[nodiscard]] static ShopifyErrorInfo fromTransport(const std::string& message);
    [[nodiscard]] static ShopifyErrorInfo fromGraphql(
        const graphql::GraphqlResponse& response,
        long http_status = 200);
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::shopify
