#include "elit21/shopify/queries/CustomerQueries.h"
namespace elit21::shopify::queries {graphql::GraphqlDocument CustomerQueries::document(){return graphql::GraphqlDocument("Customers",R"graphql(query Customers($first:Int!,$after:String){customers(first:$first,after:$after){nodes{id displayName email locale createdAt updatedAt}pageInfo{hasNextPage endCursor}}})graphql");}}
