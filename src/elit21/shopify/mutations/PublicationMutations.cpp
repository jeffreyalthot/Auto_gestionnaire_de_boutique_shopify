#include "elit21/shopify/mutations/PublicationMutations.h"
namespace elit21::shopify::mutations {graphql::GraphqlDocument PublicationMutations::document(){return graphql::GraphqlDocument("PublishablePublish",R"graphql(mutation PublishablePublish($id:ID!,$input:[PublicationInput!]!){publishablePublish(id:$id,input:$input){publishable{id} userErrors{field message}}})graphql");}}
