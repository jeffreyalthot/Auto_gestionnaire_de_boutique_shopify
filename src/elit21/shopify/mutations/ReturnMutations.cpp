#include "elit21/shopify/mutations/ReturnMutations.h"
namespace elit21::shopify::mutations {graphql::GraphqlDocument ReturnMutations::document(){return graphql::GraphqlDocument("ReturnCreate",R"graphql(mutation ReturnCreate($returnInput:ReturnInput!){returnCreate(returnInput:$returnInput){return{id} userErrors{field message}}})graphql");}}
