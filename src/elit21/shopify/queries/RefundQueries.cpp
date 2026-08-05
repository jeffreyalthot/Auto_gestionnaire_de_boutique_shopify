#include "elit21/shopify/queries/RefundQueries.h"
namespace elit21::shopify::queries {graphql::GraphqlDocument RefundQueries::document(){return graphql::GraphqlDocument("OrderRefunds",R"graphql(query OrderRefunds($id:ID!){order(id:$id){refunds{id createdAt totalRefundedSet{shopMoney{amount currencyCode}}}}})graphql");}}
