#include "elit21/shopify/queries/ApiVersionQueries.h"

namespace elit21::shopify::queries {

graphql::GraphqlDocument ApiVersionQueries::publicVersions() {
    return graphql::GraphqlDocument(
        "PublicApiVersions",
        R"graphql(
query PublicApiVersions {
  publicApiVersions {
    handle
    displayName
    supported
  }
}
)graphql");
}

} // namespace elit21::shopify::queries
