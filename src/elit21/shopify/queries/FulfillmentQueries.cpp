#include "elit21/shopify/queries/FulfillmentQueries.h"
namespace elit21::shopify::queries {graphql::GraphqlDocument FulfillmentQueries::document(){return graphql::GraphqlDocument("Fulfillment",R"graphql(query Fulfillment($id:ID!){fulfillment(id:$id){id status trackingInfo(first:10){company number url} estimatedDeliveryAt}})graphql");}}
