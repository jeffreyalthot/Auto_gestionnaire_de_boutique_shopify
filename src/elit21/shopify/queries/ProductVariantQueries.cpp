#include "elit21/shopify/queries/ProductVariantQueries.h"
namespace elit21::shopify::queries {graphql::GraphqlDocument ProductVariantQueries::document(){return graphql::GraphqlDocument("ProductVariant",R"graphql(query ProductVariant($id:ID!){productVariant(id:$id){id title sku price inventoryQuantity product{id}}})graphql");}}
