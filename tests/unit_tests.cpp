#include "elit21/compliance/CanadaCompliance.h"
#include "elit21/config/Config.h"
#include "elit21/pricing/PricingEngine.h"
#include "elit21/pricing/ShopifyFeeEstimator.h"
#include "elit21/returns/ShopifyRefundExecutor.h"
#include "elit21/terminal/panels/ShopifyPanel.h"
#include "elit21/security/Crypto.h"
#include "elit21/shopify/ShopifyMoney.h"
#include "elit21/shopify/ShopifyAppConfigGenerator.h"
#include "elit21/shopify/auth/ShopifyOAuthCallbackVerifier.h"
#include "elit21/shopify/ShopifyOrderAdmissionService.h"
#include "elit21/shopify/ShopifyOrderStateMachine.h"
#include "elit21/shopify/ShopifyPrivacyService.h"
#include "elit21/shopify/ShopifyProductionReadiness.h"
#include "elit21/shopify/ShopifyApiVersionGuard.h"
#include "elit21/shopify/ShopifyGlobalId.h"
#include "elit21/shopify/ShopifyIdempotencyKey.h"
#include "elit21/shopify/ShopifyIdempotencyStore.h"
#include "elit21/shopify/ShopifyTimestamp.h"
#include "elit21/shopify/ShopifyWebhookReliabilityMonitor.h"
#include "elit21/shopify/ShopifyApiContractAuditor.h"
#include "elit21/shopify/ShopifyInventoryWritePlanner.h"
#include "elit21/shopify/ShopifyProductSetSyncService.h"
#include "elit21/shopify/graphql/ShopifyGraphqlCircuitBreaker.h"
#include "elit21/storage/Database.h"
#include "elit21/util/StringUtil.h"

#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <thread>

namespace {
int failures = 0;

void expect(bool value, const std::string& message) {
    if (!value) {
        std::cerr << "ECHEC: " << message << '\n';
        ++failures;
    } else {
        std::cout << "PASS: " << message << '\n';
    }
}

std::string readFile(const std::string& path) {
    std::ifstream input(path, std::ios::binary);
    std::ostringstream output;
    output << input.rdbuf();
    return output.str();
}

elit21::Address canadianAddress() {
    elit21::Address address;
    address.first_name = "Jeffrey";
    address.last_name = "Test";
    address.address1 = "1 rue Exemple";
    address.city = "Montreal";
    address.province = "Quebec";
    address.province_code = "QC";
    address.country = "Canada";
    address.country_code = "CA";
    address.postal_code = "H2X 1Y4";
    return address;
}

elit21::CustomerOrder mappedOrder(const std::string& id) {
    elit21::CustomerOrder order;
    order.shopify_order_id = id;
    order.order_name = "#" + id;
    order.email = "customer@example.test";
    order.currency = "CAD";
    order.shipping_address = canadianAddress();
    order.total_cad = 23.0;
    elit21::OrderLine line;
    line.shopify_line_id = id + "-line-1";
    line.shopify_variant_id = "gid://shopify/ProductVariant/501";
    line.aliexpress_product_id = "AE-PRODUCT-1";
    line.aliexpress_sku_id = "AE-SKU-1";
    line.sku = "SKU-1";
    line.title = "Produit de test";
    line.quantity = 1;
    line.unit_price_cad = 23.0;
    order.lines.push_back(line);
    return order;
}
} // namespace

int main() {
    elit21::PricingConfig pricing_config;
    pricing_config.markup_percent_before_shipping = 100.0;
    elit21::PricingEngine pricing(pricing_config);
    const auto quote = pricing.calculate(10.0, 3.0);
    expect(bool(quote), "calcul prix disponible");
    if (quote) {
        expect(std::fabs(quote.value().price_before_shipping_cad - 20.0) < 0.001,
               "100% avant livraison double le coût");
        expect(std::fabs(quote.value().final_price_cad - 23.0) < 0.001,
               "livraison ajoutée après la marge");
    }

    auto cost = elit21::shopify::ShopifyMoney::parse("10.005");
    auto shipping = elit21::shopify::ShopifyMoney::parse("3.00");
    expect(bool(cost) && cost.value().cents() == 1001, "monnaie Shopify arrondie en centimes");
    auto safe_price = cost && shipping
        ? elit21::shopify::ShopifyMoney::supplierPrice(cost.value(), 100.0, shipping.value())
        : elit21::Result<elit21::shopify::ShopifyMoney>::failure("input");
    expect(bool(safe_price) && safe_price.value().toDecimal() == "23.02",
           "prix fournisseur calculé sans dérive binaire");
    expect(!elit21::shopify::ShopifyMoney::parse("NaN"), "montant non numérique rejeté");
    expect(!elit21::shopify::ShopifyMoney::supplierPrice(elit21::shopify::ShopifyMoney(1000), 99.9, elit21::shopify::ShopifyMoney(300)),
           "marge inférieure à 100% rejetée");
    elit21::shopify::auth::ShopifyOAuthStateStore oauth_states;
    const auto oauth_state = oauth_states.issueForShop("elit21", "https://elit21.com/oauth/callback");
    std::map<std::string, std::string> oauth_parameters{
        {"code", "authorization-code"}, {"shop", "elit21.myshopify.com"},
        {"state", oauth_state},
        {"timestamp", std::to_string(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()))}
    };
    oauth_parameters["hmac"] = elit21::shopify::auth::ShopifyOAuthCallbackVerifier::signature(
        oauth_parameters, "0123456789abcdef0123456789abcdef");
    std::string oauth_query;
    for (const auto& [key, value] : oauth_parameters) {
        if (!oauth_query.empty()) oauth_query += "&";
        oauth_query += elit21::util::urlEncode(key) + "=" + elit21::util::urlEncode(value);
    }
    elit21::shopify::auth::ShopifyOAuthCallbackVerifier oauth_verifier(oauth_states);
    auto oauth_verified = oauth_verifier.verify(
        oauth_query, "0123456789abcdef0123456789abcdef");
    expect(bool(oauth_verified) && oauth_verified.value().callback.shop == "elit21.myshopify.com",
           "callback OAuth Shopify vérifié par HMAC, horodatage et état à usage unique");
    expect(!oauth_verifier.verify(oauth_query, "0123456789abcdef0123456789abcdef"),
           "rejeu du callback OAuth Shopify bloqué");

    elit21::shopify::ShopifyAppConfigurationSpec app_spec;
    app_spec.client_id = "client-id";
    app_spec.application_url = "https://elit21.com";
    app_spec.redirect_urls = {"https://elit21.com/oauth/callback"};
    app_spec.scopes = elit21::shopify::ShopifyScopes::parse(
        "read_products,write_products,read_orders,write_orders");
    auto app_toml = elit21::shopify::ShopifyAppConfigGenerator::generateToml(app_spec);
    expect(bool(app_toml) && app_toml.value().find("customers/data_request") != std::string::npos &&
           app_toml.value().find("orders/paid") != std::string::npos,
           "shopify.app.toml généré avec webhooks opérationnels et de conformité");
    elit21::pricing::ShopifyFeePolicy fee_policy{2.9, 0.30, 0.0, 0.0};
    auto fees = elit21::pricing::ShopifyFeeEstimator::estimate(100.0, fee_policy);
    expect(bool(fees) && std::fabs(fees.value().total_fees_cad - 3.20) < 0.001,
           "frais Shopify configurables calculés en centimes");
    auto refund = elit21::returns::ShopifyRefundExecutor::plan(
        "gid://shopify/Order/1", 100.0, 20.0, 30.0, "customer_request", true, true);
    expect(bool(refund) && std::fabs(refund.value().remaining_refundable_cad - 50.0) < 0.001,
           "plan de remboursement respecte le solde remboursable");
    expect(!elit21::returns::ShopifyRefundExecutor::plan(
        "gid://shopify/Order/1", 100.0, 90.0, 20.0, "invalid", false, true),
        "sur-remboursement Shopify bloqué");
    elit21::Json panel_json = elit21::Json::object();
    panel_json.set("connected", true); panel_json.set("readiness_score", 93);
    panel_json.set("api_version", "2026-07"); panel_json.set("pending_tasks", 4);
    auto panel = elit21::terminal::panels::ShopifyPanel::fromJson(panel_json);
    expect(panel.renderLines().size() >= 8 && panel.readiness_score == 93,
           "panneau Shopify fixe généré depuis un snapshot typé");

    expect(elit21::shopify::ShopifyOrderStateMachine::canTransition(
               elit21::shopify::ShopifyOrderState::pending_supplier,
               elit21::shopify::ShopifyOrderState::supplier_ordered),
           "transition commande fournisseur autorisée");
    expect(!elit21::shopify::ShopifyOrderStateMachine::canTransition(
               elit21::shopify::ShopifyOrderState::cancelled,
               elit21::shopify::ShopifyOrderState::supplier_ordered),
           "transition depuis commande annulée bloquée");


    auto gid = elit21::shopify::ShopifyGlobalId::parse(
        "gid://shopify/ProductVariant/501?inventory_item_id=601");
    expect(bool(gid) && gid.value().isType("ProductVariant") &&
           gid.value().numericId().value_or(0) == 501 &&
           gid.value().parameters().at("inventory_item_id") == "601",
           "GID Shopify typé analysé avec paramètres");
    auto generated_gid = elit21::shopify::ShopifyGlobalId::fromNumeric("Order", 9007199254740993ULL);
    expect(bool(generated_gid) && generated_gid.value().toString() ==
           "gid://shopify/Order/9007199254740993",
           "GID Shopify 64 bits généré sans perte de précision");
    expect(!elit21::shopify::ShopifyGlobalId::parse("https://shopify/Order/1"),
           "identifiant non GID rejeté");

    auto normalized_timestamp = elit21::shopify::ShopifyTimestamp::normalizeUtc(
        "2026-08-05T12:00:00-04:00");
    expect(bool(normalized_timestamp) && normalized_timestamp.value() ==
           "2026-08-05T16:00:00Z",
           "horodatage Shopify avec fuseau normalisé en UTC");
    auto timestamp_order = elit21::shopify::ShopifyTimestamp::compare(
        "2026-08-05T12:00:00-04:00", "2026-08-05T15:30:00Z");
    expect(bool(timestamp_order) && timestamp_order.value() > 0,
           "ordre chronologique Shopify compare les instants et non le texte");
    expect(!elit21::shopify::ShopifyTimestamp::normalizeUtc("2026-08-05 12:00:00"),
           "horodatage Shopify non RFC3339 rejeté");

    auto version = elit21::shopify::ShopifyApiVersionGuard::assess(
        "2026-07", "2026-07", "2026-08-05");
    expect(bool(version) && version.value().usable() && version.value().version_matches,
           "version Shopify 2026-07 reconnue et servie exactement");
    expect(!elit21::shopify::ShopifyApiVersionGuard::requireExactServedVersion(
        "2026-07", "2026-10"),
        "basculement silencieux de version API Shopify bloqué");

    elit21::Json idempotency_variables = elit21::Json::object();
    idempotency_variables.set("inventoryItemId", "gid://shopify/InventoryItem/601");
    idempotency_variables.set("quantity", 8);
    const auto deterministic_key = elit21::shopify::ShopifyIdempotencyKey::deterministic(
        "inventorySetQuantities", idempotency_variables, "inventory:601:8");
    expect(bool(elit21::shopify::ShopifyIdempotencyKey::validate(deterministic_key)),
           "clé d'idempotence Shopify déterministe valide");

    elit21::CanadaCompliance canada({"weapon"});
    expect(canada.validPostalCode("G5C 3T2"), "code postal Canada valide");
    expect(!canada.validPostalCode("12345"), "code postal invalide rejeté");
    expect(!canada.validateProduct("weapon test", ""), "produit interdit rejeté");

    const auto signature = elit21::crypto::aliExpressTopSign({{"app_key", "123"}, {"method", "test"}}, "secret");
    expect(signature.size() == 32, "signature MD5 TOP 32 hex");

    auto loaded = elit21::Config::load(std::string(ELIT21_SOURCE_DIR) + "/config/app.json");
    expect(bool(loaded), "configuration chargeable");
    if (loaded) {
        expect(loaded.value().pricing.markup_percent_before_shipping >= 100,
               "marge minimale verrouillée à 100%");
        expect(std::filesystem::path(loaded.value().app.database).is_absolute() &&
               std::filesystem::exists(loaded.value().migrations_dir),
               "chemins runtime résolus depuis la racine du projet");
    }


    elit21::ShopifyInventoryQuantityUpdate checked_inventory;
    checked_inventory.inventory_item_id = "gid://shopify/InventoryItem/601";
    checked_inventory.quantity = 9;
    checked_inventory.compare_quantity = 12;
    auto checked_batches = elit21::shopify::ShopifyInventoryWritePlanner::build(
        "gid://shopify/Location/701", {checked_inventory},
        elit21::shopify::ShopifyIdempotencyKey::random(),
        "elit21://inventory/test", false);
    expect(bool(checked_batches) && checked_batches.value().size() == 1 &&
           checked_batches.value()[0].variables.dump().find("changeFromQuantity") != std::string::npos &&
           checked_batches.value()[0].variables.dump().find("ignoreCompareQuantity") == std::string::npos,
           "inventaire Shopify 2026-07 utilise changeFromQuantity sans champ hérité");

    checked_inventory.compare_quantity.reset();
    auto unchecked_batches = elit21::shopify::ShopifyInventoryWritePlanner::build(
        "gid://shopify/Location/701", {checked_inventory},
        elit21::shopify::ShopifyIdempotencyKey::random(),
        "elit21://inventory/bootstrap", true);
    expect(bool(unchecked_batches) &&
           unchecked_batches.value()[0].variables.dump().find("\"changeFromQuantity\":null") != std::string::npos,
           "amorçage inventaire explicite encode changeFromQuantity null");
    expect(!elit21::shopify::ShopifyInventoryWritePlanner::build(
        "gid://shopify/Location/701", {checked_inventory},
        elit21::shopify::ShopifyIdempotencyKey::random(),
        "elit21://inventory/unsafe", false),
        "écriture absolue sans quantité de comparaison refusée par défaut");

    elit21::ShopifyProductCreate set_product;
    set_product.title = "Produit ProductSet";
    set_product.description_html = "<p>Produit synchronisé</p>";
    set_product.vendor = "ELIT21";
    set_product.product_type = "Test";
    set_product.source_product_id = "AE-PRODUCT-SET-1";
    elit21::ShopifyProductVariantCreate set_variant;
    set_variant.option_name = "Color";
    set_variant.option_value = "Blue";
    set_variant.sku = "ELIT21-BLUE-1";
    set_variant.source_product_id = set_product.source_product_id;
    set_variant.source_sku_id = "AE-SKU-BLUE-1";
    set_variant.supplier_cost_cad = 10.0;
    set_variant.price_cad = 20.0;
    set_variant.inventory = 8;
    set_product.variants.push_back(set_variant);
    auto product_set_plan = elit21::shopify::ShopifyProductSetSyncService::plan(set_product, false);
    expect(bool(product_set_plan) &&
           product_set_plan.value().variables.dump().find("aliexpress_product_id") != std::string::npos &&
           product_set_plan.value().variables.dump().find("AE-PRODUCT-SET-1") != std::string::npos &&
           product_set_plan.value().variables.getBool("synchronous", true) == false,
           "productSet prépare un upsert asynchrone identifié par métachamp externe");


    std::vector<elit21::shopify::ShopifyWebhookDeliverySample> delivery_samples;
    for (int index = 0; index < 20; ++index) {
        elit21::shopify::ShopifyWebhookDeliverySample sample;
        sample.topic = "orders/paid";
        sample.http_status = 200;
        sample.response_time_ms = 100.0 + index * 10.0;
        delivery_samples.push_back(sample);
    }
    auto webhook_slo = elit21::shopify::ShopifyWebhookReliabilityMonitor::analyze(delivery_samples);
    expect(webhook_slo.healthy && webhook_slo.p90_response_time_ms >= 260.0 &&
           webhook_slo.failure_rate_percent == 0.0,
           "p90 webhook Shopify calculé avec un SLO sain");
    delivery_samples.back().http_status = 500;
    delivery_samples.back().retry_count = 8;
    auto degraded_slo = elit21::shopify::ShopifyWebhookReliabilityMonitor::analyze(delivery_samples);
    expect(!degraded_slo.healthy && degraded_slo.failed == 1 &&
           degraded_slo.maximum_retry_count == 8,
           "échec webhook et budget de huit reprises détectés");

    elit21::shopify::graphql::ShopifyGraphqlCircuitBreaker circuit(
        2, std::chrono::milliseconds(1));
    expect(circuit.allowRequest(), "circuit GraphQL autorise la première requête");
    circuit.recordFailure();
    expect(circuit.allowRequest(), "circuit GraphQL reste fermé avant le seuil");
    circuit.recordFailure();
    expect(!circuit.allowRequest(), "circuit GraphQL s'ouvre après le seuil d'échecs");
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
    expect(circuit.allowRequest(), "circuit GraphQL autorise une sonde half-open");
    circuit.recordSuccess();
    expect(circuit.snapshot().state_name == "closed", "circuit GraphQL se referme après succès");

    const auto temporary_root = std::filesystem::temp_directory_path() /
        ("elit21-v6-tests-" + elit21::crypto::randomHex(6));
    std::filesystem::create_directories(temporary_root);
    const auto database_path = (temporary_root / "test.sqlite3").string();
    elit21::Database database;
    auto opened = database.open(database_path);
    expect(bool(opened), "base SQLite transactionnelle ouverte");
    auto migrated = database.migrateDirectory(std::string(ELIT21_SOURCE_DIR) + "/migrations");
    expect(bool(migrated), "38 migrations SQLite appliquées");

    if (migrated) {
        auto contract_report = elit21::shopify::ShopifyApiContractAuditor::audit(
            ELIT21_SOURCE_DIR, &database);
        expect(bool(contract_report) && contract_report.value().passed &&
               contract_report.value().score >= 90,
               "contrat Shopify 2026-07 audité et persisté");
        auto stored_slo = elit21::shopify::ShopifyWebhookReliabilityMonitor::analyzeAndPersist(
            delivery_samples, database);
        expect(bool(stored_slo) && stored_slo.value().deliveries == delivery_samples.size(),
               "SLO webhook Shopify persisté dans SQLite");
        elit21::StoredProduct product;
        product.ae_id = "AE-PRODUCT-1";
        product.title = "Produit de test";
        product.status = "active";
        product.cost = 10.0;
        product.shipping = 3.0;
        product.price = 23.0;
        product.stock = 10;
        expect(bool(database.upsertProduct(product)), "produit fournisseur persisté");

        elit21::StoredVariant variant;
        variant.ae_product_id = "AE-PRODUCT-1";
        variant.ae_sku_id = "AE-SKU-1";
        variant.shopify_variant_id = "gid://shopify/ProductVariant/501";
        variant.shopify_inventory_item_id = "gid://shopify/InventoryItem/601";
        variant.sku = "SKU-1";
        variant.cost = 10.0;
        variant.shipping = 3.0;
        variant.price = 23.0;
        variant.stock = 10;
        variant.status = "active";
        expect(bool(database.upsertVariant(variant)), "variante fournisseur persistée");

        variant.shopify_quantity = 7;
        expect(bool(database.upsertVariant(variant)),
               "quantité Shopify de référence persistée avec la variante");
        auto stored_variant = database.variantByAliExpressSku("AE-PRODUCT-1", "AE-SKU-1");
        expect(bool(stored_variant) && stored_variant.value().has_value() &&
               stored_variant.value()->shopify_quantity == 7,
               "quantité Shopify relue pour compare-and-set");
        expect(bool(database.updateVariantShopifyQuantity(
                   "gid://shopify/InventoryItem/601", 8)),
               "checkpoint de quantité Shopify mis à jour atomiquement");
        stored_variant = database.variantByAliExpressSku("AE-PRODUCT-1", "AE-SKU-1");
        expect(bool(stored_variant) && stored_variant.value().has_value() &&
               stored_variant.value()->shopify_quantity == 8,
               "nouvelle quantité Shopify relue après écriture");

        elit21::shopify::ShopifyIdempotencyStore idempotency_store(database);
        auto reservation = idempotency_store.reserve(
            "inventorySetQuantities", idempotency_variables, "inventory:601:8", 24);
        expect(bool(reservation) && reservation.value().newly_reserved,
               "opération Shopify réservée durablement");
        if (reservation) {
            expect(bool(idempotency_store.markInProgress(reservation.value())),
                   "opération Shopify marquée en cours");
            elit21::Json completed_response = elit21::Json::object();
            completed_response.set("quantity", 8);
            expect(bool(idempotency_store.complete(reservation.value(), completed_response)),
                   "résultat Shopify idempotent mis en cache");
            auto repeated = idempotency_store.reserve(
                "inventorySetQuantities", idempotency_variables, "inventory:601:8", 24);
            expect(bool(repeated) && repeated.value().completed() &&
                   !repeated.value().newly_reserved,
                   "rejeu idempotent retourne le résultat déjà complété");
        }
        elit21::Json changed_variables = idempotency_variables;
        changed_variables.set("quantity", 9);
        auto reused_key = database.reserveShopifyIdempotency(
            deterministic_key, "inventorySetQuantities",
            elit21::shopify::ShopifyIdempotencyKey::parameterHash(changed_variables), 24);
        expect(!reused_key,
               "réutilisation d'une clé Shopify avec paramètres différents rejetée");

        auto first_event = database.evaluateShopifyWebhookOrder(
            "elit21.myshopify.com", "orders", "9001",
            "2026-08-05T15:00:00Z", "event-new", "webhook-new", "hash-new");
        expect(bool(first_event) && first_event.value().accepted(),
               "premier webhook Shopify accepté pour la ressource");
        auto duplicate_event = database.evaluateShopifyWebhookOrder(
            "elit21.myshopify.com", "orders", "9001",
            "2026-08-05T15:00:00Z", "event-new", "webhook-duplicate", "hash-new");
        expect(bool(duplicate_event) && duplicate_event.value().decision ==
               elit21::ShopifyWebhookOrderDecision::duplicate,
               "webhook Shopify dupliqué détecté par Event-Id");
        auto stale_event = database.evaluateShopifyWebhookOrder(
            "elit21.myshopify.com", "orders", "9001",
            "2026-08-05T14:59:00Z", "event-old", "webhook-old", "hash-old");
        expect(bool(stale_event) && stale_event.value().decision ==
               elit21::ShopifyWebhookOrderDecision::stale,
               "webhook Shopify plus ancien détecté sans écraser l'état récent");

        auto offset_event = database.evaluateShopifyWebhookOrder(
            "elit21.myshopify.com", "orders", "offset-order",
            "2026-08-05T12:00:00-04:00", "event-offset-new",
            "webhook-offset-new", "hash-offset-new");
        auto offset_stale = database.evaluateShopifyWebhookOrder(
            "elit21.myshopify.com", "orders", "offset-order",
            "2026-08-05T15:30:00Z", "event-offset-old",
            "webhook-offset-old", "hash-offset-old");
        expect(bool(offset_event) && offset_event.value().accepted() &&
               bool(offset_stale) && offset_stale.value().decision ==
                   elit21::ShopifyWebhookOrderDecision::stale,
               "webhook Shopify périmé détecté malgré des fuseaux différents");
        expect(!database.evaluateShopifyWebhookOrder(
            "elit21.myshopify.com", "orders", "invalid-time",
            "not-a-date", "event-invalid", "webhook-invalid", "hash-invalid"),
            "horodatage de webhook invalide rejeté avant mutation d'état");
        auto reconciliation = database.requestShopifyReconciliation(
            "elit21.myshopify.com", "orders", "9001", "stale_webhook",
            "event-old", "webhook-old", "unit_test");
        auto reconciliation_duplicate = database.requestShopifyReconciliation(
            "elit21.myshopify.com", "orders", "9001", "stale_webhook",
            "event-old", "webhook-old", "unit_test");
        expect(bool(reconciliation) && reconciliation.value().inserted &&
               bool(reconciliation_duplicate) && !reconciliation_duplicate.value().inserted,
               "réconciliation Shopify dédupliquée dans la file durable");

        const auto reconciliation_key = "shopify-reconcile:" + elit21::crypto::sha256Hex(
            std::string("elit21.myshopify.com\norders\n9001\nstale_webhook\nevent-old"));
        expect(bool(database.updateShopifyReconciliationStatus(
                   reconciliation_key, "running")) &&
               bool(database.updateShopifyReconciliationStatus(
                   reconciliation_key, "completed")),
               "cycle de vie de réconciliation Shopify persisté");


        elit21::InventoryConfig inventory;
        inventory.safety_buffer = 2;
        elit21::RiskConfig risk;
        risk.maximum_order_value_cad = 500.0;
        elit21::shopify::ShopifyOrderAdmissionService admission(database, pricing_config, inventory, risk);
        auto admitted = admission.admit(mappedOrder("9007199254740993"), true, true);
        expect(bool(admitted) && admitted.value().accepted(), "commande rentable admise transactionnellement");
        auto duplicate = admission.admit(mappedOrder("9007199254740993"), true, true);
        expect(bool(duplicate) && duplicate.value().decision ==
               elit21::shopify::ShopifyOrderAdmissionDecision::duplicate,
               "commande Shopify dupliquée dédupliquée");

        auto manual = mappedOrder("9007199254740994");
        manual.total_cad = 15.0;
        manual.lines[0].shopify_line_id = "manual-line";
        manual.lines[0].aliexpress_product_id.clear();
        manual.lines[0].aliexpress_sku_id.clear();
        manual.lines[0].sku = "UNMAPPED-SKU";
        manual.lines[0].unit_price_cad = 15.0;
        auto reviewed = admission.admit(manual, true, true);
        expect(bool(reviewed) && reviewed.value().decision ==
               elit21::shopify::ShopifyOrderAdmissionDecision::manual_review,
               "commande sans mappage conservée en révision manuelle");
        auto review_orders = database.ordersByStatus("manual_review", 20);
        expect(bool(review_orders) && !review_orders.value().empty(),
               "commande en anomalie persistée pour audit");

        variant.stock = 2;
        expect(bool(database.upsertVariant(variant)), "stock fournisseur mis à jour avant prévalidation");
        auto stale_evaluation = admission.evaluate(mappedOrder("future-order"), true, true);
        expect(bool(stale_evaluation) && !stale_evaluation.value().accepted(),
               "stock égal au tampon de sécurité bloque la commande fournisseur");
        variant.stock = 10;
        expect(bool(database.upsertVariant(variant)), "stock fournisseur restauré");

        auto transitioned = database.transitionOrderStatus(
            "9007199254740993", {"pending_supplier"}, "supplier_ordered",
            "supplier_order_created", "{\"dry_run\":true}");
        expect(bool(transitioned) && transitioned.value(), "transition d'état atomique enregistrée");

        elit21::shopify::ShopifyPrivacyService privacy(database, temporary_root.string());
        elit21::Json request = elit21::Json::object();
        request.set("webhook_id", "privacy-data-1");
        request.set("topic", "customers/data_request");
        request.set("payload_hash", elit21::crypto::sha256Hex("privacy-data-body"));
        request.set("payload",
            "{\"shop_domain\":\"elit21.myshopify.com\",\"customer\":{\"id\":\"42\"},"
            "\"orders_requested\":[\"9007199254740993\"]}");
        auto exported = privacy.processTask("shopify_customers_data_request", request);
        expect(bool(exported) && std::filesystem::exists(exported.value().export_path),
               "export de confidentialité écrit atomiquement");
        if (exported) expect(readFile(exported.value().export_path).find("customer@example.test") != std::string::npos,
                             "export contient les données demandées avant effacement");

        elit21::Json redact = elit21::Json::object();
        redact.set("webhook_id", "privacy-redact-1");
        redact.set("topic", "customers/redact");
        redact.set("payload_hash", elit21::crypto::sha256Hex("privacy-redact-body"));
        redact.set("payload",
            "{\"shop_domain\":\"elit21.myshopify.com\",\"customer\":{\"id\":\"42\"},"
            "\"orders_to_redact\":[\"9007199254740993\"]}");
        auto redacted = privacy.processTask("shopify_customers_redact", redact);
        expect(bool(redacted) && redacted.value().affected_orders == 1,
               "données client Shopify effacées");
        auto redacted_order = database.ordersByShopifyIds({"9007199254740993"});
        expect(bool(redacted_order) && redacted_order.value().size() == 1 &&
               redacted_order.value()[0].customer_email.empty(),
               "adresse courriel supprimée de la base");

        elit21::Config production;
        production.app.name = "ELIT21";
        production.app.live_orders = true;
        production.app.dry_run = false;
        production.network.verify_tls = true;
        production.shopify.shop = "elit21";
        production.shopify.api_version = "2026-07";
        production.shopify.access_token = "shpat_abcdefghijklmnopqrstuvwxyz";
        production.shopify.webhook_secret = "0123456789abcdefghijklmnopqrstuvwxyz";
        production.shopify.webhook_base_url = "https://elit21.com";
        production.aliexpress.app_key = "app-key";
        production.aliexpress.app_secret = "app-secret";
        production.aliexpress.access_token = "access-token";
        production.aliexpress.country = "CA";
        production.aliexpress.currency = "CAD";
        production.pricing.markup_percent_before_shipping = 100.0;
        auto readiness = elit21::shopify::ShopifyProductionReadiness::evaluate(production, &database);
        expect(readiness.production_ready && readiness.score >= 90,
               "préparation production Shopify évaluée avec critères critiques");
    }

    std::error_code remove_error;
    std::filesystem::remove_all(temporary_root, remove_error);
    std::cout << (failures ? "TESTS EN ECHEC" : "TOUS LES TESTS SONT REUSSIS") << '\n';
    return failures ? 1 : 0;
}
