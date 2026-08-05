#include "elit21/shopify/mutations/DraftOrderMutations.h"
namespace elit21::shopify::mutations {graphql::GraphqlDocument DraftOrderMutations::document(){return graphql::GraphqlDocument("DraftOrderCreate",R"graphql(mutation DraftOrderCreate($input:DraftOrderInput!){draftOrderCreate(input:$input){draftOrder{id} userErrors{field message}}})graphql");}}
