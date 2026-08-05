#include "elit21/shopify/mutations/FulfillmentMutations.h"
namespace elit21::shopify::mutations {graphql::GraphqlDocument FulfillmentMutations::document(){return graphql::GraphqlDocument("FulfillmentCreate",R"graphql(mutation FulfillmentCreate($fulfillment:FulfillmentInput!){fulfillmentCreate(fulfillment:$fulfillment){fulfillment{id status} userErrors{field message}}})graphql");}}
