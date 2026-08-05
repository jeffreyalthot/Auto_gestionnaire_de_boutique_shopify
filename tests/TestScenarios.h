#pragma once

#include "elit21/aliexpress/AliExpressMethodNames.h"
#include "elit21/aliexpress/AliExpressModels.h"
#include "elit21/aliexpress/requests/FreightCalculateRequest.h"
#include "elit21/aliexpress/requests/PlaceOrderRequest.h"
#include "elit21/aliexpress/transport/TopRequestSigner.h"
#include "elit21/compliance/CanadaCompliance.h"
#include "elit21/config/Config.h"
#include "elit21/finance/NetProfitCalculator.h"
#include "elit21/fulfillment/TrackingEventNormalizer.h"
#include "elit21/inventory/InventorySafetyBuffer.h"
#include "elit21/json/Json.h"
#include "elit21/mapping/ProductMappingService.h"
#include "elit21/net/HttpClient.h"
#include "elit21/orders/OrderValidationService.h"
#include "elit21/platform/ManagedComponent.h"
#include "elit21/pricing/MarginCalculator.h"
#include "elit21/pricing/PricingEngine.h"
#include "elit21/resilience/RetryPolicy.h"
#include "elit21/returns/RefundCalculator.h"
#include "elit21/risk/RiskRuleEngine.h"
#include "elit21/security/Crypto.h"
#include "elit21/security/DataEncryptor.h"
#include "elit21/security/DataRedactor.h"
#include "elit21/shopify/ShopifyWebhook.h"
#include "elit21/shopify/ShopifyApiVersion.h"
#include "elit21/shopify/ShopifyAutonomyEngine.h"
#include "elit21/shopify/ShopifyConfig.h"
#include "elit21/shopify/ShopifyClient.h"
#include "elit21/shopify/ShopifyErrorMapper.h"
#include "elit21/shopify/ShopifyRateLimitState.h"
#include "elit21/shopify/ShopifyServiceFactory.h"
#include "elit21/shopify/ShopifyEndpoints.h"
#include "elit21/shopify/ShopifyApiVersionCatalog.h"
#include "elit21/shopify/ShopifyIdempotencyStore.h"
#include "elit21/shopify/ShopifyMutationCoordinator.h"
#include "elit21/shopify/ShopifyOperationalGovernance.h"
#include "elit21/shopify/ShopifyScopes.h"
#include "elit21/shopify/auth/ShopifyAuthorizationUrlBuilder.h"
#include "elit21/shopify/auth/ShopifyOAuthStateStore.h"
#include "elit21/shopify/auth/ShopifyTokenManager.h"
#include "elit21/shopify/services/ShopifyOrderService.h"
#include "elit21/shopify/services/ShopifyGovernedMutationService.h"
#include "elit21/shopify/ShopifyWebhookProcessor.h"
#include "elit21/shopify/ShopifyWebhookRegistry.h"
#include "elit21/shopify/graphql/ShopifyCostThrottler.h"
#include "elit21/shopify/ShopifyModels.h"
#include "elit21/shopify/graphql/GraphqlDocument.h"
#include "elit21/shopify/graphql/GraphqlRequest.h"
#include "elit21/shopify/graphql/GraphqlResponse.h"
#include "elit21/shopify/graphql/ShopifyGraphqlPaginator.h"
#include "elit21/shopify/graphql/ShopifyQueryBuilder.h"
#include "elit21/shopify/queries/ApiVersionQueries.h"
#include "elit21/shopify/bulk/BulkJsonlParser.h"
#include "elit21/shopify/bulk/StagedUploadManager.h"
#include "elit21/shopify/webhooks/ProductWebhookHandler.h"
#include "elit21/shopify/webhooks/ShopifyWebhookDispatcher.h"
#include "elit21/shopify/webhooks/ShopifyWebhookParser.h"
#include "elit21/shopify/webhooks/ShopifyWebhookReceiver.h"
#include "elit21/sourcing/ProductScorer.h"
#include "elit21/storage/Database.h"
#include "elit21/terminal/ProgressBar.h"
#include "elit21/workflow/WorkflowEngine.h"

#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <map>
#include <string>
#include <thread>
#include <tuple>

namespace elit21::tests {

inline platform::OperationContext context(std::string payload, bool dry_run = true) {
    platform::OperationContext value;
    value.correlation_id = crypto::randomHex(8);
    value.payload = std::move(payload);
    value.dry_run = dry_run;
    return value;
}

template <class Component>
inline bool executeComponent(const std::string& payload,
                             const std::map<std::string, double>& expected_metrics = {}) {
    Component component;
    component.configure({{"country", "CA"}, {"currency", "CAD"}, {"credentials_ready", "true"}});
    if (!component.start()) return false;
    const auto result = component.tick(context(payload));
    const auto snapshot = component.snapshot();
    component.stop();
    if (!result.success || snapshot.processed != 1 || snapshot.accepted != 1) return false;
    for (const auto& [name, value] : expected_metrics) {
        const auto found = result.metrics.find(name);
        if (found == result.metrics.end() || std::fabs(found->second - value) > 0.01) return false;
    }
    return true;
}

inline bool pricingScenario() {
    PricingConfig configuration;
    configuration.markup_percent_before_shipping = 100.0;
    PricingEngine engine(configuration);
    const auto price = engine.calculate(10.0, 3.0);
    if (!price || std::fabs(price.value().price_before_shipping_cad - 20.0) > 0.001 ||
        std::fabs(price.value().final_price_cad - 23.0) > 0.001) return false;
    return executeComponent<pricing::MarginCalculator>(
        R"({"supplier_cost_cad":10,"shipping_cad":3,"markup_percent":20})",
        {{"price_before_shipping_cad", 20.0}, {"final_price_cad", 23.0}});
}

inline bool inventoryScenario() {
    return executeComponent<inventory::InventorySafetyBuffer>(
        R"({"supplier_stock":12,"safety_buffer":2})", {{"available_to_sell", 10.0}});
}

inline bool orderScenario() {
    CustomerOrder model;
    model.shopify_order_id = "gid://shopify/Order/1";
    model.currency = "CAD";
    model.total_cad = 23.0;
    model.shipping_address.first_name = "Jean";
    model.shipping_address.last_name = "Test";
    model.shipping_address.address1 = "1 Rue Principale";
    model.shipping_address.city = "Montreal";
    model.shipping_address.province_code = "QC";
    model.shipping_address.country_code = "CA";
    model.shipping_address.postal_code = "H2Y 1C6";
    OrderLine model_line;
    model_line.sku = "AE-1005001-sku1";
    model_line.quantity = 1;
    model_line.unit_price_cad = 23.0;
    model.lines.push_back(model_line);
    if (!model.valid()) return false;
    if (!executeComponent<orders::OrderValidationService>(
            R"({"order_total_cad":125.50,"quantity":3})",
            {{"order_total_cad", 125.5}, {"line_or_quantity_count", 3.0}})) return false;
    orders::OrderValidationService component;
    component.configure({});
    component.start();
    const auto rejected = component.tick(context(R"({"order_total_cad":-1,"quantity":1})"));
    component.stop();
    return !rejected.success && rejected.code == "INVALID_ORDER";
}

inline bool complianceScenario() {
    CanadaCompliance compliance({"weapon", "counterfeit"});
    Address address;
    address.address1 = "100 Rue Principale";
    address.city = "Montreal";
    address.province_code = "QC";
    address.country_code = "CA";
    address.postal_code = "H2Y 1C6";
    if (!compliance.validateAddress(address, false)) return false;
    if (compliance.validateProduct("Counterfeit product", "").ok()) return false;
    return compliance.validPostalCode("G5C 3T2") && !compliance.validPostalCode("12345");
}

inline bool financeScenario() {
    return executeComponent<finance::NetProfitCalculator>(
        R"({"revenue_cad":100,"supplier_cost_cad":30,"shipping_cad":10,"fees_cad":5})",
        {{"net_profit_cad", 55.0}});
}

inline bool mappingScenario() {
    return executeComponent<mapping::ProductMappingService>(
        R"({"source_id":"AE-123","target_id":"gid://shopify/Product/456"})");
}

inline bool fulfillmentScenario() {
    return executeComponent<fulfillment::TrackingEventNormalizer>(
        R"({"tracking_number":"CA123456789CN","carrier":"Canada Post"})");
}

inline bool workflowScenario() {
    return executeComponent<workflow::WorkflowEngine>(
        R"({"workflow_id":"order-1","state":"pending","next_state":"supplier_ordered"})");
}

inline bool returnsScenario() {
    return executeComponent<returns::RefundCalculator>(
        R"({"order_id":"1","amount_cad":12.50,"reason":"damaged"})");
}

inline bool riskScenario() {
    risk::RiskRuleEngine engine;
    engine.configure({});
    engine.start();
    auto accepted = engine.tick(context(R"({"order_total_cad":100,"maximum_order_value_cad":750})"));
    auto rejected = engine.tick(context(R"({"order_total_cad":751,"maximum_order_value_cad":750})"));
    engine.stop();
    return accepted.success && !rejected.success && rejected.code == "RISK_LIMIT_EXCEEDED";
}

inline bool sourcingScenario() {
    return executeComponent<sourcing::ProductScorer>(
        R"({"product_id":"1005001","rating":4.8,"seller_score":97,"orders":500})");
}

inline bool terminalScenario() {
    return executeComponent<terminal::ProgressBar>(R"({"progress":0.75,"label":"sync"})");
}

inline bool retryScenario() {
    resilience::RetryPolicy policy;
    policy.configure({});
    policy.start();
    auto result = policy.tick(context(R"({"attempt":2,"max_attempts":5,"retry_after_seconds":10})"));
    policy.stop();
    return result.success;
}

inline bool graphqlScenario() {
    ShopifyProductCreate product;
    product.title = "Produit";
    product.sku = "AE-1005001-sku1";
    product.source_product_id = "1005001";
    product.supplier_cost_cad = 10.0;
    product.price_cad = 20.0;
    product.inventory = 5;
    ShopifyProductVariantCreate blue;
    blue.sku = "AE-1005001-blue";
    blue.source_product_id = "1005001";
    blue.source_sku_id = "blue";
    blue.option_name = "Color";
    blue.option_value = "Blue";
    blue.supplier_cost_cad = 10.0;
    blue.price_cad = 20.0;
    blue.inventory = 5;
    ShopifyProductVariantCreate red = blue;
    red.sku = "AE-1005001-red";
    red.source_sku_id = "red";
    red.option_value = "Red";
    product.variants = {blue, red};
    if (!product.valid() || !blue.valid() || !red.valid()) return false;
    ShopifyOrderPage page;
    page.orders.push_back(CustomerOrder{});
    page.has_next_page = true;
    page.end_cursor = "cursor-2";
    if (!page.cursorConsistent()) return false;
    shopify::graphql::ShopifyCostThrottler throttler;
    auto extension = Json::parse(R"({"cost":{"actualQueryCost":90,"throttleStatus":{"currentlyAvailable":10,"restoreRate":50}}})");
    if (!extension) return false;
    throttler.update(extension.value());
    if (std::fabs(throttler.available() - 10.0) > 0.01 || throttler.recommendedDelay(60).count() < 900) return false;
    shopify::graphql::GraphqlDocument document(
        "ShopIdentity", "query ShopIdentity { shop { id name myshopifyDomain } }");
    if (!document.validate() || document.isMutation()) return false;
    shopify::graphql::GraphqlRequest request(document, Json::object());
    auto request_json = Json::parse(request.dump());
    if (!request_json || request_json.value().getString("operationName") != "ShopIdentity") return false;
    auto response = shopify::graphql::GraphqlResponse::parse(
        R"({"data":{"shop":{"id":"gid://shopify/Shop/1","name":"ELIT21"}},"extensions":{"cost":{"requestedQueryCost":1}}})");
    if (!response || response.value().hasErrors() ||
        response.value().data().get("shop").getString("name") != "ELIT21") return false;
    auto error = shopify::graphql::GraphqlResponse::parse(
        R"({"errors":[{"message":"throttled","extensions":{"code":"THROTTLED"}}]})");
    return error && error.value().hasErrors() && error.value().combinedErrorMessage().find("throttled") != std::string::npos;
}



inline bool shopifyInfrastructureScenario() {
    using namespace shopify::graphql;
    GraphqlDocument documented(
        "CatalogPage",
        "# comment\nquery CatalogPage($first:Int!,$after:String){products(first:$first,after:$after){nodes{id title}pageInfo{hasNextPage endCursor}}}");
    if (!documented.validate() || documented.declaredVariables().size() != 2 ||
        documented.normalizedSource().find("# comment") != std::string::npos ||
        documented.fingerprint().empty()) return false;
    Json variables = Json::object();
    variables.set("first", 50);
    GraphqlRequest graph_request(documented, variables);
    if (!graph_request.validate() || graph_request.idempotencyKey().empty()) return false;

    ShopifyGraphqlPaginator paginator(3);
    auto page1 = Json::parse(R"({"hasNextPage":true,"endCursor":"cursor-1"})");
    auto page2 = Json::parse(R"({"hasNextPage":true,"endCursor":"cursor-1"})");
    if (!page1 || !page2 || !paginator.update(page1.value()) || paginator.pageCount() != 1 ||
        paginator.update(page2.value()).ok()) return false;
    if (!ShopifyQueryBuilder::validateGid("gid://shopify/Product/123", "Product") ||
        ShopifyQueryBuilder::validateGid("gid://shopify/Order/123", "Product").ok() ||
        !ShopifyQueryBuilder::safeFieldEquals("status", "open") ||
        ShopifyQueryBuilder::safeFieldEquals("bad field", "open") ||
        !ShopifyQueryBuilder::safeUpdatedAfter("2026-08-05T12:00:00Z") ||
        ShopifyQueryBuilder::safeUpdatedAfter("yesterday")) return false;

    auto graph_response = GraphqlResponse::parse(
        R"({"data":{"shop":{"id":"gid://shopify/Shop/1"}},"extensions":{"cost":{"requestedQueryCost":7,"actualQueryCost":5,"throttleStatus":{"currentlyAvailable":995}}}})");
    if (!graph_response || graph_response.value().dataAt({"shop"}).getString("id").empty() ||
        std::fabs(graph_response.value().actualCost() - 5.0) > 0.01) return false;

    ::elit21::ShopifyConfig raw_config;
    raw_config.shop = "elit21-test";
    raw_config.access_token = "shpat_012345678901234567890123456789";
    raw_config.api_version = "2026-07";
    raw_config.webhook_secret = "0123456789abcdef0123456789abcdef";
    raw_config.webhook_base_url = "https://elit21.com";
    shopify::ShopifyConfig typed_config(raw_config);
    if (!typed_config.validate(true) || typed_config.adminEndpoint().find("graphql.json") == std::string::npos ||
        typed_config.sanitizedSummary().getString("shop") != "elit21-test") return false;
    auto mapped = shopify::ShopifyErrorMapper::fromHttp(429, R"({"errors":"Throttled"})", 2);
    if (!mapped.retryable || mapped.suggested_retry_seconds != 2 || mapped.category != shopify::ShopifyErrorCategory::throttled) return false;
    shopify::ShopifyRateLimitState limit_state;
    shopify::graphql::GraphqlTransportMetrics transport_metrics;
    transport_metrics.requests = 2;
    transport_metrics.retries = 1;
    transport_metrics.throttles = 1;
    transport_metrics.currently_available_cost = 40;
    limit_state.update(transport_metrics);
    if (limit_state.healthy(100) || limit_state.delayFor(90).count() <= 0 ||
        limit_state.snapshot().getNumber("requests") != 2) return false;
    HttpClient factory_http;
    shopify::ShopifyServiceFactory factory(raw_config, factory_http);
    if (!factory.ready() || !factory.validate() || !factory.createClient()) return false;

    shopify::bulk::BulkJsonlParser parser;
    std::size_t consumed = 0;
    auto bulk = parser.parseText(
        "{\"id\":\"gid://shopify/Product/1\",\"title\":\"A\"}\n"
        "{\"id\":\"gid://shopify/ProductVariant/2\",\"__parentId\":\"gid://shopify/Product/1\",\"sku\":\"SKU-2\"}\n",
        [&](const shopify::bulk::BulkJsonlRecord& record) {
            if (record.id.empty()) return Result<void>::failure("missing id");
            ++consumed;
            return Result<void>::success();
        });
    if (!bulk || bulk.value().objects != 2 || bulk.value().children != 1 || consumed != 2) return false;

    auto staged_json = Json::parse(R"({"stagedTargets":[{"url":"https://uploads.example.test/","resourceUrl":"https://cdn.example.test/file","parameters":[{"name":"key","value":"value"}]}]})");
    if (!staged_json) return false;
    auto staged = shopify::bulk::StagedUploadManager::parseTarget(staged_json.value());
    if (!staged || !staged.value().valid()) return false;

    namespace fs = std::filesystem;
    const auto directory = fs::temp_directory_path() / ("elit21-webhooks-v5-" + crypto::randomHex(8));
    fs::create_directories(directory);
    bool success = false;
    {
        Database database;
        if (!database.open((directory / "webhooks.db").string()) ||
            !database.migrateDirectory(std::string(ELIT21_SOURCE_DIR) + "/migrations")) return false;
        shopify::webhooks::ProductWebhookHandler product_handler(database);
        shopify::webhooks::ShopifyWebhookDispatcher dispatcher;
        dispatcher.registerHandler(shopify::webhooks::ShopifyWebhookTopic::products_update,
            [&](const shopify::webhooks::ShopifyWebhook& event) { return product_handler.handle(event); });
        const std::string secret = "webhook-secret-v5";
        shopify::webhooks::ShopifyWebhookReceiver receiver(secret, dispatcher);
        IncomingRequest incoming;
        incoming.method = "POST";
        incoming.path = "/webhooks/shopify";
        incoming.body = R"({"id":9007199254740993,"title":"Produit"})";
        incoming.headers["x-shopify-topic"] = "products/update";
        incoming.headers["x-shopify-shop-domain"] = "elit21-test.myshopify.com";
        incoming.headers["x-shopify-api-version"] = "2026-07";
        incoming.headers["x-shopify-webhook-id"] = "webhook-product-v5";
        incoming.headers["x-shopify-hmac-sha256"] = crypto::hmacSha256Base64(secret, incoming.body);
        const auto accepted = receiver.receive(incoming);
        if (accepted.status != 202) return false;
        auto tasks = database.claimTasks("shopify-infrastructure-v5", 10);
        if (!tasks || tasks.value().size() != 1 || tasks.value()[0].kind != "shopify_product_updated") return false;
        incoming.headers["x-shopify-hmac-sha256"] = "invalid";
        if (receiver.receive(incoming).status != 401) return false;
        success = true;
    }
    std::error_code error;
    fs::remove_all(directory, error);
    return success;
}

inline bool shopifyAutonomyScenario() {
    using namespace shopify;
    using namespace shopify::auth;

    auto version = ShopifyApiVersion::parse("2026-07");
    if (!version || version.value().quarter() != 3 ||
        version.value().nextQuarter().value() != "2026-10" ||
        version.value().previousQuarter().value() != "2026-04") return false;
    if (ShopifyEndpoints::normalizeShop("https://ELIT21-Test.myshopify.com/admin") != "elit21-test" ||
        !ShopifyEndpoints::validateShop("elit21-test") ||
        ShopifyEndpoints::validateShop("bad_shop")) return false;

    auto required = ShopifyScopes::autonomousManagerDefaults();
    auto granted = ShopifyScopes::parse(required.commaSeparated());
    if (!granted.containsAll(required) || !required.missingFrom(granted).empty()) return false;

    ShopifyOAuthStateStore state_store;
    const auto state = state_store.issueForShop("elit21-test", "https://elit21.com/oauth/callback");
    auto consumed = state_store.consumeDetails(state);
    if (!consumed || consumed->shop != "elit21-test" || state_store.consume(state)) return false;

    ShopifyAuthorizationRequest authorization;
    authorization.shop = "elit21-test";
    authorization.client_id = "client-id";
    authorization.scopes = required;
    authorization.redirect_uri = "https://elit21.com/oauth/callback";
    authorization.state = std::string(64, 'a');
    auto authorization_url = ShopifyAuthorizationUrlBuilder::build(authorization);
    if (!authorization_url || authorization_url.value().find("admin/oauth/authorize") == std::string::npos) return false;

    namespace fs = std::filesystem;
    const auto directory = fs::temp_directory_path() / ("elit21-shopify-v5-" + crypto::randomHex(8));
    fs::create_directories(directory);
    bool success = false;
    {
        ShopifyTokenManager tokens;
        ShopifyAccessToken token;
        token.access_token = "shpat_012345678901234567890123456789";
        token.scope = required.commaSeparated();
        tokens.store(token);
        const auto token_path = (directory / "shopify.token").string();
        const std::string master = "0123456789abcdef0123456789abcdef";
        if (!tokens.hasScopes(required) || tokens.fingerprint().size() != 16 ||
            !tokens.sanitizedMetadata().getBool("ready") || !tokens.saveEncrypted(token_path, master)) return false;
        tokens.clear();
        if (tokens.ready() || !tokens.loadEncrypted(token_path, master) || !tokens.ready()) return false;

        Database database;
        if (!database.open((directory / "autonomy.db").string()) ||
            !database.migrateDirectory(std::string(ELIT21_SOURCE_DIR) + "/migrations")) return false;
        StoredProduct source_product;
        source_product.ae_id = "1005001";
        source_product.title = "Produit source";
        source_product.status = "active";
        source_product.cost = 10.0;
        source_product.shipping = 3.0;
        source_product.price = 23.0;
        source_product.stock = 10;
        if (!database.upsertProduct(source_product)) return false;
        StoredVariant source_variant;
        source_variant.ae_product_id = "1005001";
        source_variant.ae_sku_id = "blue";
        source_variant.shopify_variant_id = "gid://shopify/ProductVariant/1";
        source_variant.shopify_inventory_item_id = "gid://shopify/InventoryItem/1";
        source_variant.sku = "AE-1005001-blue";
        source_variant.cost = 10.0;
        source_variant.shipping = 3.0;
        source_variant.price = 23.0;
        source_variant.stock = 10;
        source_variant.status = "active";
        if (!database.upsertVariant(source_variant)) return false;

        ::elit21::ShopifyConfig config;
        config.shop = "elit21-test";
        config.access_token = token.access_token;
        config.webhook_secret = "0123456789abcdef0123456789abcdef";
        config.webhook_base_url = "https://elit21.com";
        HttpClient http;
        ShopifyClient client(config, http);
        ShopifyAutonomyEngine engine(client, database, config);
        ShopifyAutonomyReport report;
        report.dry_run = true;
        CustomerOrder order;
        order.shopify_order_id = "gid://shopify/Order/5001";
        order.order_name = "#5001";
        order.currency = "CAD";
        order.total_cad = 23.0;
        order.shipping_address.first_name = "Jean";
        order.shipping_address.last_name = "Test";
        order.shipping_address.address1 = "1 Rue Principale";
        order.shipping_address.city = "Montreal";
        order.shipping_address.province_code = "QC";
        order.shipping_address.country_code = "CA";
        order.shipping_address.postal_code = "H2Y 1C6";
        OrderLine line;
        line.shopify_line_id = "gid://shopify/LineItem/1";
        line.shopify_variant_id = "gid://shopify/ProductVariant/1";
        line.sku = "AE-1005001-blue";
        line.aliexpress_product_id = "1005001";
        line.aliexpress_sku_id = "blue";
        line.quantity = 1;
        line.unit_price_cad = 23.0;
        order.lines.push_back(line);
        order.raw_json = R"({"id":"gid://shopify/Order/5001"})";
        auto first = engine.ingestPaidOrders({order}, true, report);
        auto second = engine.ingestPaidOrders({order}, true, report);
        if (!first || first.value() != 1 || !second || second.value() != 0 ||
            report.new_orders_persisted != 1 || report.duplicate_orders != 1 || report.tasks_enqueued != 1) return false;
        auto stored = database.ordersByStatus("pending_supplier", 10);
        auto tasks = database.claimTasks("shopify-v5-test", 10);
        if (!stored || stored.value().size() != 1 || !tasks || tasks.value().size() != 1 ||
            tasks.value()[0].kind != "place_supplier_order") return false;
        shopify::services::ShopifyOrderService order_service(client);
        if (!order_service.bound()) return false;
        success = true;
    }
    std::error_code error;
    fs::remove_all(directory, error);
    return success;
}

inline bool aliExpressScenario() {
    AliProduct product;
    product.product_id = "1005001";
    product.title = "Produit";
    product.min_price_cad = 10.0;
    product.max_price_cad = 12.0;
    product.stock = 5;
    product.skus.push_back({"sku1", "AE-1005001-sku1", 10.0, 5});
    FreightQuote quote{"AliExpress Standard", "CAINIAO_STANDARD", 3.0, 20, true, true};
    if (!product.valid() || !quote.valid()) return false;
    if (!aliexpress::AliExpressMethodNames::supported(aliexpress::AliExpressMethodNames::productGet()) ||
        aliexpress::AliExpressMethodNames::all().size() < 10) return false;
    auto freight = aliexpress::requests::FreightCalculateRequest::create(
        {{"param_aeop_freight_calculate_for_buyer_d_t_o", R"({"product_id":"1005001","sku_id":"sku-1","quantity":1,"country_code":"CA"})"}});
    if (!freight.validate()) return false;
    auto order = aliexpress::requests::PlaceOrderRequest::create(
        {{"param_place_order_request4_open_api_d_t_o", R"({"logistics_address":{},"product_items":[]})"}});
    if (!order.validate()) return false;
    const auto signature = aliexpress::transport::TopRequestSigner::signMd5(
        {{"app_key", "123"}, {"method", aliexpress::AliExpressMethodNames::productGet()}}, "secret");
    return signature.size() == 32;
}

inline bool webhookScenario() {
    ShopifyConfig configuration;
    configuration.webhook_secret = "test-secret";
    ShopifyWebhook webhook(configuration);
    const std::string body = R"({"id":9007199254740993,"name":"#1001","email":"client@example.com","phone":"","currency":"CAD","current_total_price":"23.00","shipping_address":{"first_name":"Jean","last_name":"Test","address1":"1 Rue Principale","city":"Montreal","province":"Quebec","province_code":"QC","country_code":"CA","zip":"H2Y 1C6"},"line_items":[{"id":9007199254740995,"variant_id":9007199254740997,"sku":"AE-1005001-sku1","title":"Produit","price":"23.00","quantity":1,"properties":[{"name":"_aliexpress_product_id","value":"1005001"},{"name":"_aliexpress_sku_id","value":"sku1"}]}]})";
    IncomingRequest request;
    request.method = "POST";
    request.path = "/webhooks/shopify/orders-paid";
    request.body = body;
    request.headers["x-shopify-hmac-sha256"] = crypto::hmacSha256Base64(configuration.webhook_secret, body);
    if (!webhook.verify(request)) return false;
    auto order = webhook.parseOrderPaid(body);
    if (!order || order.value().shopify_order_id != "9007199254740993" || order.value().lines.size() != 1 ||
        order.value().lines[0].shopify_line_id != "9007199254740995" ||
        order.value().lines[0].shopify_variant_id != "9007199254740997" ||
        order.value().lines[0].aliexpress_product_id != "1005001" ||
        order.value().lines[0].aliexpress_sku_id != "sku1") return false;
    const auto invalid = webhook.parseOrderPaid(
        R"({"id":1,"currency":"CAD","current_total_price":"not-a-number","shipping_address":{},"line_items":[{"id":1,"quantity":1}]})");
    if (invalid) return false;

    namespace fs = std::filesystem;
    const auto directory = fs::temp_directory_path() / ("elit21-webhook-" + crypto::randomHex(8));
    fs::create_directories(directory);
    bool processor_success = false;
    {
        Database database;
        if (!database.open((directory / "webhook.db").string()) ||
            !database.migrateDirectory(std::string(ELIT21_SOURCE_DIR) + "/migrations")) return false;
        configuration.shop = "elit21-test";
        ShopifyWebhookProcessor processor(configuration, database);
        request.path = "/webhooks/shopify";
        request.headers["x-shopify-shop-domain"] = "elit21-test.myshopify.com";
        request.headers["x-shopify-topic"] = "orders/paid";
        request.headers["x-shopify-webhook-id"] = "webhook-advanced-1";
        auto accepted = processor.handle(request);
        auto duplicate = processor.handle(request);
        if (accepted.status != 202 || duplicate.status != 200) return false;
        auto tasks = database.claimTasks("test-worker", 10);
        if (!tasks || tasks.value().size() != 1 || tasks.value()[0].kind != "shopify_order_paid") return false;
        request.headers["x-shopify-hmac-sha256"] = "invalid";
        if (processor.handle(request).status != 401) return false;
        request.headers["x-shopify-hmac-sha256"] = crypto::hmacSha256Base64(configuration.webhook_secret, body);
        request.headers["x-shopify-shop-domain"] = "other.myshopify.com";
        if (processor.handle(request).status != 403) return false;
        processor_success = true;
    }
    std::error_code cleanup_error;
    fs::remove_all(directory, cleanup_error);
    return processor_success;
}

inline bool databaseScenario() {
    namespace fs = std::filesystem;
    const auto directory = fs::temp_directory_path() / ("elit21-test-" + crypto::randomHex(8));
    fs::create_directories(directory);
    const auto path = directory / "test.db";
    bool success = false;
    {
        Database database;
        if (!database.open(path.string())) return false;
        if (!database.migrateDirectory(std::string(ELIT21_SOURCE_DIR) + "/migrations")) return false;
        StoredProduct product;
        product.ae_id = "1005001";
        product.title = "Produit test";
        product.cost = 10;
        product.shipping = 3;
        product.price = 23;
        product.stock = 8;
        product.status = "candidate";
        if (!database.upsertProduct(product)) return false;
        StoredVariant variant;
        variant.ae_product_id = product.ae_id;
        variant.ae_sku_id = "sku1";
        variant.sku = "AE-1005001-sku1";
        variant.cost = 10;
        variant.shipping = 3;
        variant.price = 23;
        variant.stock = 8;
        variant.status = "active";
        if (!database.upsertVariant(variant)) return false;
        auto variants = database.variantsForProduct(product.ae_id);
        if (!variants || variants.value().size() != 1) return false;
        StoredOrder order;
        order.shopify_id = "123";
        order.status = "pending_supplier";
        order.payload = "{}";
        order.total = 23;
        order.idempotency_key = "idempotency-123";
        auto first = database.saveOrder(order);
        auto duplicate = database.saveOrder(order);
        if (!first || !first.value() || !duplicate || duplicate.value()) return false;
        if (!database.updateOrderStatus("123", "cancelled", "customer_request")) return false;
        auto cancelled = database.ordersByStatus("cancelled", 10);
        if (!cancelled || cancelled.value().size() != 1) return false;
        if (!database.setSyncCheckpoint("orders", "cursor-123", R"({"pages":2})", false)) return false;
        auto checkpoint = database.syncCheckpoint("orders");
        if (!checkpoint || !checkpoint.value() || checkpoint.value()->find("cursor-123") == std::string::npos) return false;
        auto webhook_first = database.recordWebhook("shopify", "webhook-1", "orders/paid", "hash");
        auto webhook_duplicate = database.recordWebhook("shopify", "webhook-1", "orders/paid", "hash");
        if (!webhook_first || !webhook_first.value() || !webhook_duplicate || webhook_duplicate.value()) return false;
        if (!database.markWebhookProcessed("shopify", "webhook-1", "temporary")) return false;
        auto webhook_retry = database.recordWebhook("shopify", "webhook-1", "orders/paid", "hash");
        if (!webhook_retry || !webhook_retry.value()) return false;
        auto webhook_tamper = database.recordWebhook("shopify", "webhook-1", "orders/paid", "different-hash");
        if (webhook_tamper) return false;
        auto unique_first = database.enqueueUniqueTask("unique", R"({"id":1})", 5, "", "unique:test");
        auto unique_second = database.enqueueUniqueTask("unique", R"({"id":1})", 5, "", "unique:test");
        if (!unique_first || !unique_first.value().inserted || !unique_second || unique_second.value().inserted ||
            unique_first.value().id != unique_second.value().id) return false;
        auto task = database.enqueueTask("test", R"({"id":1})", 10);
        if (!task) return false;
        auto claimed = database.claimTasks("worker-1", 10);
        if (!claimed || claimed.value().size() != 2) return false;
        for (const auto& claimed_task : claimed.value()) {
            if (!database.completeTask(claimed_task.id)) return false;
        }
        auto unique_after_completion = database.enqueueUniqueTask("unique", R"({"id":2})", 5, "", "unique:test");
        if (!unique_after_completion || !unique_after_completion.value().inserted) return false;
        if (!database.execute("UPDATE tasks SET status='processing',locked_by='dead-worker',locked_at='2000-01-01T00:00:00Z' WHERE id=" +
                              std::to_string(unique_after_completion.value().id))) return false;
        auto recovered = database.recoverStaleTasks(1);
        if (!recovered || recovered.value() != 1) return false;
        auto recovered_tasks = database.claimTasks("worker-recovery", 10);
        if (!recovered_tasks || recovered_tasks.value().empty()) return false;
        for (const auto& recovered_task : recovered_tasks.value()) {
            if (!database.completeTask(recovered_task.id)) return false;
        }
        if (!database.audit("INFO", "test", "audit chain")) return false;
        success = true;
    }
    const auto checksum_directory = directory / "checksum-migrations";
    fs::create_directories(checksum_directory);
    const auto migration = checksum_directory / "0001_test.sql";
    {
        std::ofstream output(migration);
        output << "CREATE TABLE checksum_test(id INTEGER PRIMARY KEY);\n";
    }
    {
        Database checksum_database;
        if (!checksum_database.open((directory / "checksum.db").string()) ||
            !checksum_database.migrateDirectory(checksum_directory.string())) return false;
        {
            std::ofstream output(migration, std::ios::trunc);
            output << "CREATE TABLE checksum_test(id INTEGER PRIMARY KEY, value TEXT);\n";
        }
        if (checksum_database.migrateDirectory(checksum_directory.string())) return false;
    }
    std::error_code error;
    fs::remove_all(directory, error);
    return success;
}


inline bool shopifyGovernanceScenario() {
    Json catalog_root = Json::object();
    Json catalog_data = Json::object();
    Json versions = Json::array();
    for (const auto& item : std::vector<std::tuple<std::string, std::string, bool>>{
             {"2026-04", "2026-04", true},
             {"2026-07", "2026-07", true},
             {"2026-10", "2026-10 Release Candidate", false},
             {"unstable", "unstable", false}}) {
        Json version = Json::object();
        version.set("handle", std::get<0>(item));
        version.set("displayName", std::get<1>(item));
        version.set("supported", std::get<2>(item));
        versions.push(version);
    }
    catalog_data.set("publicApiVersions", versions);
    catalog_root.set("data", catalog_data);
    auto version_document = shopify::queries::ApiVersionQueries::publicVersions();
    if (!version_document.validate() || version_document.operationName() != "PublicApiVersions") return false;
    auto catalog = shopify::ShopifyApiVersionCatalog::parse(catalog_root, "elit21.myshopify.com");
    if (!catalog || !catalog.value().supports("2026-07") ||
        catalog.value().latestSupported().value_or("") != "2026-07" ||
        catalog.value().nextAfter("2026-04").value_or("") != "2026-07") return false;
    if (!shopify::ShopifyApiVersionCatalog::requireSupported(catalog.value(), "2026-07") ||
        shopify::ShopifyApiVersionCatalog::requireSupported(catalog.value(), "2026-10")) return false;

    namespace fs = std::filesystem;
    const auto directory = fs::temp_directory_path() / ("elit21-governance-" + crypto::randomHex(6));
    fs::create_directories(directory);
    Database database;
    if (!database.open((directory / "governance.sqlite3").string()) ||
        !database.migrateDirectory(std::string(ELIT21_SOURCE_DIR) + "/migrations")) return false;
    if (!shopify::ShopifyApiVersionCatalog::persist(database, catalog.value())) return false;
    auto loaded = shopify::ShopifyApiVersionCatalog::load(database, "elit21.myshopify.com");
    if (!loaded || !loaded.value() || !loaded.value()->supports("2026-07")) return false;

    ShopifyConfig client_config;
    client_config.shop = "elit21.myshopify.com";
    client_config.api_version = "2026-07";
    HttpClient http;
    ShopifyClient client(client_config, http);
    Json variables = Json::object();
    variables.set("input", Json::object());
    shopify::ShopifyIdempotencyStore store(database);
    auto handle = store.reserve("GovernedMutation", variables, "business-1", 24);
    if (!handle || !store.markInProgress(handle.value())) return false;
    Json cached = Json::object();
    Json cached_data = Json::object();
    cached_data.set("ok", true);
    cached.set("data", cached_data);
    if (!store.complete(handle.value(), cached)) return false;

    shopify::ShopifyMutationCoordinator coordinator(client, database);
    auto replay = coordinator.execute(
        "GovernedMutation",
        "mutation GovernedMutation($input: String, $idempotencyKey: String!) { governedMutation(input: $input) @idempotent(key: $idempotencyKey) { userErrors { field message } } }",
        variables,
        "business-1");
    if (!replay || !replay.value().replayed || !replay.value().response.get("data").getBool("ok")) return false;
    shopify::services::ShopifyGovernedMutationService mutation_service(client, database);
    auto service_replay = mutation_service.executeMutation(
        "GovernedMutation",
        "mutation GovernedMutation($input: String, $idempotencyKey: String!) { governedMutation(input: $input) @idempotent(key: $idempotencyKey) { userErrors { field message } } }",
        variables,
        "business-1");
    if (!service_replay || !service_replay.value().replayed || !mutation_service.bound()) return false;
    if (coordinator.execute("NotMutation", "query NotMutation { shop { id } }", Json::object(), "business-2")) return false;

    Config config;
    config.app.dry_run = true;
    config.app.live_orders = false;
    config.app.database = (directory / "governance.sqlite3").string();
    config.shopify.shop = "elit21.myshopify.com";
    config.shopify.api_version = "2026-07";
    config.shopify.webhook_base_url = "https://elit21.com";
    config.pricing.markup_percent_before_shipping = 100.0;
    config.aliexpress.country = "CA";
    config.aliexpress.currency = "CAD";
    auto report = shopify::ShopifyOperationalGovernance::evaluateOffline(config, &database);
    auto history = database.latestShopifyGovernanceRun(config.shopify.shop);
    const bool history_ok = history && history.value().has_value() &&
        history.value()->find("production_ready") != std::string::npos;
    std::error_code cleanup_error;
    fs::remove_all(directory, cleanup_error);
    return report.score >= 80 && !report.checks.empty() && history_ok;
}

inline bool securityScenario() {
    const auto hmac = crypto::hmacSha256Base64("key", "payload");
    if (hmac.empty() || !crypto::constantTimeEquals(hmac, hmac) || crypto::constantTimeEquals(hmac, hmac + "x")) return false;
    const auto random_a = crypto::randomHex(32);
    const auto random_b = crypto::randomHex(32);
    if (random_a.size() != 64 || random_b.size() != 64 || random_a == random_b) return false;
    const std::string master = "0123456789abcdef0123456789abcdef";
    auto encrypted = security::DataEncryptor::encrypt("shopify-secret", master, "test");
    if (!encrypted) return false;
    auto decrypted = security::DataEncryptor::decrypt(encrypted.value(), master, "test");
    if (!decrypted || decrypted.value() != "shopify-secret") return false;
    if (security::DataEncryptor::decrypt(encrypted.value(), master, "wrong")) return false;
    const auto redacted = security::DataRedactor::redact("access_token=abc123 Authorization: Bearer token client@example.com");
    return redacted.find("abc123") == std::string::npos && redacted.find("token") == std::string::npos &&
           crypto::sha256Hex("abc").size() == 64 && crypto::md5Hex("abc").size() == 32;
}

inline bool configurationScenario() {
    auto config = Config::load(std::string(ELIT21_SOURCE_DIR) + "/config/app.json");
    if (!config || config.value().pricing.markup_percent_before_shipping < 100.0 ||
        config.value().aliexpress.country != "CA" || config.value().aliexpress.currency != "CAD") return false;
    auto invalid = config.value();
    invalid.pricing.markup_percent_before_shipping = 99.0;
    return !invalid.validate();
}

inline bool performanceScenario() {
    const auto start = std::chrono::steady_clock::now();
    for (int index = 0; index < 2000; ++index) {
        if (!pricingScenario()) return false;
    }
    return std::chrono::steady_clock::now() - start < std::chrono::seconds(10);
}

} // namespace elit21::tests
