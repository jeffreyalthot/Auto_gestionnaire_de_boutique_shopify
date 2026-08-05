#include "elit21/shopify/queries/CollectionQueries.h"
namespace elit21::shopify::queries {graphql::GraphqlDocument CollectionQueries::document(){return graphql::GraphqlDocument("Collections",R"graphql(query Collections($first:Int!,$after:String){collections(first:$first,after:$after){nodes{id title handle updatedAt}pageInfo{hasNextPage endCursor}}})graphql");}}
