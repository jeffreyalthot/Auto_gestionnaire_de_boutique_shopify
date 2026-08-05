#include "elit21/shopify/mutations/CustomerMutations.h"
namespace elit21::shopify::mutations {graphql::GraphqlDocument CustomerMutations::document(){return graphql::GraphqlDocument("CustomerUpdate",R"graphql(mutation CustomerUpdate($input:CustomerInput!){customerUpdate(input:$input){customer{id} userErrors{field message}}})graphql");}}
