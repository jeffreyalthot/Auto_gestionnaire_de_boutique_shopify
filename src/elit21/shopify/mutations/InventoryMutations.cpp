#include "elit21/shopify/mutations/InventoryMutations.h"
#include "elit21/shopify/ShopifyInventoryWritePlanner.h"

namespace elit21::shopify::mutations {

graphql::GraphqlDocument InventoryMutations::document() {
    return graphql::GraphqlDocument(
        "InventorySetQuantities",
        ShopifyInventoryWritePlanner::mutationDocument());
}

} // namespace elit21::shopify::mutations
