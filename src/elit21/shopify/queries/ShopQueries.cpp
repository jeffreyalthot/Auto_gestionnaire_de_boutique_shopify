#include "elit21/shopify/queries/ShopQueries.h"
namespace elit21::shopify::queries {graphql::GraphqlDocument ShopQueries::document(){return graphql::GraphqlDocument("ShopIdentity",R"graphql(query ShopIdentity { shop { id name myshopifyDomain currencyCode timezoneAbbreviation } })graphql");}}
