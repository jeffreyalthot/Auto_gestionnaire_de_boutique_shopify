#include "elit21/shopify/queries/LocationQueries.h"
namespace elit21::shopify::queries {graphql::GraphqlDocument LocationQueries::document(){return graphql::GraphqlDocument("Locations",R"graphql(query Locations($first:Int!){locations(first:$first){nodes{id name isActive address{countryCode provinceCode city zip}}}})graphql");}}
