#include "elit21/shopify/mutations/RefundMutations.h"
namespace elit21::shopify::mutations {graphql::GraphqlDocument RefundMutations::document(){return graphql::GraphqlDocument("RefundCreate",R"graphql(mutation RefundCreate($input:RefundInput!){refundCreate(input:$input){refund{id} userErrors{field message}}})graphql");}}
