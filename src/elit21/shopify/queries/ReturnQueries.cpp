#include "elit21/shopify/queries/ReturnQueries.h"
namespace elit21::shopify::queries {graphql::GraphqlDocument ReturnQueries::document(){return graphql::GraphqlDocument("OrderReturns",R"graphql(query OrderReturns($id:ID!){order(id:$id){returns(first:20){nodes{id status name}}}})graphql");}}
