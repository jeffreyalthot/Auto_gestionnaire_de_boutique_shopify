#include "elit21/shopify/mutations/OrderMutations.h"
namespace elit21::shopify::mutations {graphql::GraphqlDocument OrderMutations::document(){return graphql::GraphqlDocument("OrderUpdate",R"graphql(mutation OrderUpdate($input:OrderInput!){orderUpdate(input:$input){order{id} userErrors{field message}}})graphql");}}
