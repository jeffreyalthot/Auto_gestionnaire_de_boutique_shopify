#include "elit21/shopify/queries/WebhookQueries.h"
namespace elit21::shopify::queries {graphql::GraphqlDocument WebhookQueries::document(){return graphql::GraphqlDocument("Webhooks",R"graphql(query Webhooks($first:Int!){webhookSubscriptions(first:$first){nodes{id topic endpoint{... on WebhookHttpEndpoint{callbackUrl}}}}})graphql");}}
