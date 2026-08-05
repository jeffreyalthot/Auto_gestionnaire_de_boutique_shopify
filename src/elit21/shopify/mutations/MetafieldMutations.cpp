#include "elit21/shopify/mutations/MetafieldMutations.h"
namespace elit21::shopify::mutations {graphql::GraphqlDocument MetafieldMutations::document(){return graphql::GraphqlDocument("MetafieldsSet",R"graphql(mutation MetafieldsSet($metafields:[MetafieldsSetInput!]!){metafieldsSet(metafields:$metafields){metafields{id namespace key value} userErrors{field message}}})graphql");}}
