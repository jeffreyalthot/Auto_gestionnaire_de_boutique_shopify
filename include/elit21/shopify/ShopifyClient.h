#pragma once

#include "elit21/config/Config.h"
#include "elit21/core/Result.h"
#include "elit21/core/Types.h"
#include "elit21/json/Json.h"
#include "elit21/net/HttpClient.h"
#include "elit21/shopify/ShopifyModels.h"
#include "elit21/shopify/graphql/GraphqlTransport.h"

#include <string>
#include <vector>

namespace elit21 {

class ShopifyClient {
public:
    ShopifyClient(ShopifyConfig config, HttpClient& http);

    Result<Json> graphql(const std::string& query, const Json& variables, int max_attempts = 4);
    [[nodiscard]] shopify::graphql::GraphqlTransportMetrics apiMetrics() const;

    Result<void> healthCheck();
    Result<ShopifyOrderPage> fetchOpenPaidOrdersPage(int first = 100,
                                                     const std::string& after_cursor = {});
    Result<std::vector<CustomerOrder>> fetchOpenPaidOrders(int maximum_orders = 100,
                                                           int max_pages = 10);

    Result<std::string> createProduct(const ShopifyProductCreate& product);
    Result<std::vector<ShopifyVariantRef>> createVariants(
        const std::string& product_gid,
        const std::vector<ShopifyProductVariantCreate>& variants,
        bool remove_standalone_variant = true);
    Result<void> updateVariantPrice(const std::string& product_gid,
                                    const std::string& variant_gid,
                                    double price_cad);
    Result<void> updateVariantPrices(const std::string& product_gid,
                                     const std::vector<ShopifyVariantPriceUpdate>& updates);
    Result<void> updateInventory(const std::string& inventory_item_gid,
                                 const std::string& location_gid,
                                 int quantity);
    Result<void> updateInventories(const std::string& location_gid,
                                   const std::vector<ShopifyInventoryQuantityUpdate>& updates);
    Result<void> updateInventoriesIdempotent(
        const std::string& location_gid,
        const std::vector<ShopifyInventoryQuantityUpdate>& updates,
        const std::string& idempotency_key,
        const std::string& reference_document_uri,
        bool allow_unchecked = false);
    Result<std::vector<ShopifyInventoryQuantitySnapshot>> inventoryQuantitiesAtLocation(
        const std::string& location_gid,
        const std::vector<std::string>& inventory_item_gids);
    Result<std::vector<ShopifyVariantRef>> productVariants(const std::string& product_gid);
    Result<std::string> primaryLocationId();

    Result<std::vector<ShopifyFulfillmentOrderRef>> fulfillmentOrders(const std::string& order_gid);
    Result<void> createFulfillment(const std::string& fulfillment_order_gid,
                                   const ShipmentInfo& tracking,
                                   bool notify_customer = true);

    Result<void> registerWebhook(const std::string& topic, const std::string& callback_url);
    Result<std::vector<ShopifyWebhookSubscriptionRef>> webhookSubscriptions(int first = 100);
    Result<void> deleteWebhook(const std::string& subscription_gid);

    Result<std::string> primaryPublicationId();
    Result<void> publishProduct(const std::string& product_gid, const std::string& publication_gid);

    Result<ShopifyBulkOperationRef> runBulkQuery(const std::string& bulk_query);
    Result<std::vector<ShopifyBulkOperationRef>> bulkOperations(int first = 20,
                                                                const std::string& filter = {});
    Result<ShopifyBulkOperationRef> bulkOperation(const std::string& operation_gid);
    Result<void> cancelBulkOperation(const std::string& operation_gid);

    const ShopifyConfig& config() const { return config_; }

private:
    CustomerOrder parseOrderNode(const Json& node) const;
    Result<void> ensureNoUserErrors(const Json& mutation_payload, const std::string& operation) const;
    static Json variantInput(const ShopifyProductVariantCreate& variant);
    static ShopifyVariantRef parseVariantRef(const Json& node);
    static ShopifyBulkOperationRef parseBulkOperation(const Json& node);

    ShopifyConfig config_;
    HttpClient& http_;
    shopify::graphql::GraphqlTransport transport_;
};

} // namespace elit21
