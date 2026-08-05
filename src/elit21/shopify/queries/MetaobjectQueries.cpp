#include "elit21/shopify/queries/MetaobjectQueries.h"
namespace elit21::shopify::queries {graphql::GraphqlDocument MetaobjectQueries::document(){return graphql::GraphqlDocument("Metaobjects",R"graphql(query Metaobjects($type:String!,$first:Int!){metaobjects(type:$type,first:$first){nodes{id handle type updatedAt}}})graphql");}}
