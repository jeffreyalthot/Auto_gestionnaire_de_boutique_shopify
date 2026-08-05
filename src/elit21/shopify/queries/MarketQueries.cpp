#include "elit21/shopify/queries/MarketQueries.h"
namespace elit21::shopify::queries {graphql::GraphqlDocument MarketQueries::document(){return graphql::GraphqlDocument("Markets",R"graphql(query Markets($first:Int!){markets(first:$first){nodes{id name status primary enabled}}})graphql");}}
