#pragma once

#include "elit21/shopify/graphql/GraphqlOperation.h"
#include "elit21/shopify/graphql/GraphqlTransport.h"

#include <cstdint>

namespace elit21::shopify::graphql {

struct ShopifyGraphqlClientMetrics {
    std::uint64_t operations{0};
    std::uint64_t queries{0};
    std::uint64_t mutations{0};
    std::uint64_t failures{0};
};

class ShopifyGraphqlClient {
public:
    explicit ShopifyGraphqlClient(GraphqlTransport& transport) : transport_(transport) {}

    Result<Json> execute(const GraphqlDocument& document, const Json& variables = Json::object());
    Result<Json> execute(const GraphqlOperation& operation);
    Result<Json> executeAt(const GraphqlDocument& document, const Json& variables,
                           const std::vector<std::string>& data_path);
    [[nodiscard]] ShopifyGraphqlClientMetrics metrics() const noexcept { return metrics_; }

private:
    GraphqlTransport& transport_;
    ShopifyGraphqlClientMetrics metrics_;
};

} // namespace elit21::shopify::graphql
