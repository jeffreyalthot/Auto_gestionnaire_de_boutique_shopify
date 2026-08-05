#include "elit21/shopify/mutations/MetaobjectMutations.h"
namespace elit21::shopify::mutations {graphql::GraphqlDocument MetaobjectMutations::document(){return graphql::GraphqlDocument("MetaobjectCreate",R"graphql(mutation MetaobjectCreate($metaobject:MetaobjectCreateInput!){metaobjectCreate(metaobject:$metaobject){metaobject{id handle} userErrors{field message}}})graphql");}}
