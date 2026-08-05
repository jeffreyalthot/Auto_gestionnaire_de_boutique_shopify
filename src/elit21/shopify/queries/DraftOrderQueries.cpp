#include "elit21/shopify/queries/DraftOrderQueries.h"
namespace elit21::shopify::queries {graphql::GraphqlDocument DraftOrderQueries::document(){return graphql::GraphqlDocument("DraftOrders",R"graphql(query DraftOrders($first:Int!){draftOrders(first:$first){nodes{id name status totalPriceSet{shopMoney{amount currencyCode}}}}})graphql");}}
