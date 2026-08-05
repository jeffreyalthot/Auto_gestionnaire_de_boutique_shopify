#include "elit21/shopify/queries/PublicationQueries.h"
namespace elit21::shopify::queries {graphql::GraphqlDocument PublicationQueries::document(){return graphql::GraphqlDocument("Publications",R"graphql(query Publications($first:Int!){publications(first:$first){nodes{id name supportsFuturePublishing}}})graphql");}}
