#include "elit21/shopify/queries/MetafieldQueries.h"
namespace elit21::shopify::queries {graphql::GraphqlDocument MetafieldQueries::document(){return graphql::GraphqlDocument("Metafields",R"graphql(query Metafields($owner:ID!,$first:Int!){node(id:$owner){... on HasMetafields{metafields(first:$first){nodes{id namespace key type value}}}}})graphql");}}
