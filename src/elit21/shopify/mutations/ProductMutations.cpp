#include "elit21/shopify/mutations/ProductMutations.h"
namespace elit21::shopify::mutations {graphql::GraphqlDocument ProductMutations::document(){return graphql::GraphqlDocument("ProductCreate",R"graphql(mutation ProductCreate($product:ProductCreateInput!){productCreate(product:$product){product{id} userErrors{field message}}})graphql");}}
