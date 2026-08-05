#include "elit21/shopify/mutations/FileMutations.h"
namespace elit21::shopify::mutations {graphql::GraphqlDocument FileMutations::document(){return graphql::GraphqlDocument("FileCreate",R"graphql(mutation FileCreate($files:[FileCreateInput!]!){fileCreate(files:$files){files{id fileStatus} userErrors{field message}}})graphql");}}
