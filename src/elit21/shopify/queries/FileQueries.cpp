#include "elit21/shopify/queries/FileQueries.h"
namespace elit21::shopify::queries {graphql::GraphqlDocument FileQueries::document(){return graphql::GraphqlDocument("Files",R"graphql(query Files($first:Int!,$after:String){files(first:$first,after:$after){nodes{id fileStatus alt createdAt}pageInfo{hasNextPage endCursor}}})graphql");}}
