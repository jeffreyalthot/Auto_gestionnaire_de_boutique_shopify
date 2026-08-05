#include "elit21/shopify/queries/DiscountQueries.h"
namespace elit21::shopify::queries {graphql::GraphqlDocument DiscountQueries::document(){return graphql::GraphqlDocument("DiscountNodes",R"graphql(query DiscountNodes($first:Int!){discountNodes(first:$first){nodes{id discount{... on DiscountCodeBasic{title status}}}}})graphql");}}
