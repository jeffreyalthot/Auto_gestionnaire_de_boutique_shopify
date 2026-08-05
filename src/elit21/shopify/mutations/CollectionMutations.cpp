#include "elit21/shopify/mutations/CollectionMutations.h"
namespace elit21::shopify::mutations {graphql::GraphqlDocument CollectionMutations::document(){return graphql::GraphqlDocument("CollectionCreate",R"graphql(mutation CollectionCreate($input:CollectionInput!){collectionCreate(input:$input){collection{id} userErrors{field message}}})graphql");}}
