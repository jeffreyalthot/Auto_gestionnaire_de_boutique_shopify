from pathlib import Path
root=Path('/mnt/data/ELIT21_Shopify_AliExpress_Autonomous_Manager-v6.0.0-SHOPIFY-TRANSACTIONAL')

def write(rel,text):
 p=root/rel;p.parent.mkdir(parents=True,exist_ok=True);p.write_text(text,encoding='utf-8')

def replace(rel,old,new,count=-1):
 p=root/rel;s=p.read_text(encoding='utf-8')
 if old not in s: raise SystemExit(f'missing {rel}: {old[:100]!r}')
 p.write_text(s.replace(old,new,count),encoding='utf-8')

replace('include/elit21/storage/Database.h',
'''    Result<void> updateOrderSupplierId(const std::string& shopify_id,
                                       const std::string& aliexpress_id,
                                       const std::string& status);
''',
'''    Result<void> updateOrderSupplierId(const std::string& shopify_id,
                                       const std::string& aliexpress_id,
                                       const std::string& status);
    Result<bool> setSupplierOrderAndTransition(const std::string& shopify_id,
                                               const std::string& expected_status,
                                               const std::string& aliexpress_id,
                                               const std::string& next_status,
                                               const std::string& context_json = "{}");
''')

marker='''Result<void> Database::updateOrderSupplierId(const std::string& shopify_id,'''
method=r'''Result<bool> Database::setSupplierOrderAndTransition(const std::string& shopify_id,
                                                            const std::string& expected_status,
                                                            const std::string& aliexpress_id,
                                                            const std::string& next_status,
                                                            const std::string& context_json) {
    if (shopify_id.empty() || expected_status.empty() || aliexpress_id.empty() || next_status.empty())
        return Result<bool>::failure("Supplier-order transition parameters are incomplete");
    auto valid = shopify::ShopifyOrderStateMachine::validateTransition(expected_status, next_status);
    if (!valid) return Result<bool>::failure(valid.error());
    std::lock_guard lock(mutex_);
    if (!db_) return Result<bool>::failure("Database is not open");
    auto begin = executeUnlocked("BEGIN IMMEDIATE;");
    if (!begin) return Result<bool>::failure(begin.error());
    const auto now = util::utcNowIso();
    sqlite3_stmt* statement = nullptr;
    const char* sql = "UPDATE orders SET aliexpress_order_id=?,status=?,last_error='',updated_at=? "
                      "WHERE shopify_order_id=? AND status=?";
    if (sqlite3_prepare_v2(db_, sql, -1, &statement, nullptr) != SQLITE_OK) {
        executeUnlocked("ROLLBACK;"); return Result<bool>::failure(sqlite3_errmsg(db_));
    }
    sqlite3_bind_text(statement, 1, aliexpress_id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 2, next_status.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 3, now.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 4, shopify_id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 5, expected_status.c_str(), -1, SQLITE_TRANSIENT);
    const int code = sqlite3_step(statement);
    const bool changed = sqlite3_changes(db_) > 0;
    const auto error = code == SQLITE_DONE ? std::string{} : std::string(sqlite3_errmsg(db_));
    sqlite3_finalize(statement);
    if (code != SQLITE_DONE) { executeUnlocked("ROLLBACK;"); return Result<bool>::failure(error); }
    if (!changed) {
        auto commit = executeUnlocked("COMMIT;");
        return commit ? Result<bool>::success(false) : Result<bool>::failure(commit.error());
    }
    const auto event = executeUnlocked(
        "INSERT INTO shopify_order_events(order_id,event_type,from_status,to_status,context_json,created_at) "
        "SELECT id,'supplier_order_created'," + sqlQuote(expected_status) + ',' + sqlQuote(next_status) + ',' +
        sqlQuote(context_json) + ',' + sqlQuote(now) + " FROM orders WHERE shopify_order_id=" + sqlQuote(shopify_id));
    if (!event) { executeUnlocked("ROLLBACK;"); return Result<bool>::failure(event.error()); }
    auto commit = executeUnlocked("COMMIT;");
    return commit ? Result<bool>::success(true) : Result<bool>::failure(commit.error());
}

'''
replace('src/elit21/storage/Database.cpp',marker,method+marker)

# Missing supplier mappings are now routed to manual review instead of being dropped.
replace('src/elit21/orders/OrderManager.cpp',
'''        if (line.aliexpress_product_id.empty() || line.aliexpress_sku_id.empty()) {
            return Result<void>::failure("SKU sans mappage AliExpress: " + line.sku);
        }
''',
'''        if (line.aliexpress_product_id.empty() || line.aliexpress_sku_id.empty()) {
            log_.warning("orders", "SKU sans mappage AliExpress; révision manuelle requise: " + line.sku);
        }
''')

# Add a final admission check immediately before supplier placement and use atomic state transition.
replace('src/elit21/orders/OrderManager.cpp',
'''        auto mapping = resolveMappings(order);
        if (!mapping) {
            ++counters_.errors;
            log_.error("orders", mapping.error());
            continue;
        }

        if (config_.app.dry_run || !config_.app.live_orders) {
''',
'''        auto mapping = resolveMappings(order);
        if (!mapping) {
            ++counters_.errors;
            log_.error("orders", mapping.error());
            continue;
        }

        auto final_admission = admission_.evaluate(order, config_.app.dry_run || !config_.app.live_orders, true);
        if (!final_admission) {
            ++counters_.errors;
            db_.transitionOrderStatus(stored.shopify_id, {"pending_supplier"}, "manual_review",
                                      "supplier_preflight_failed", "{}", final_admission.error());
            log_.error("orders", "Prévalidation fournisseur échouée: " + final_admission.error());
            continue;
        }
        if (!final_admission.value().accepted()) {
            Json review = final_admission.value().toJson();
            auto transition = db_.transitionOrderStatus(stored.shopify_id, {"pending_supplier"}, "manual_review",
                                                        "supplier_preflight_review", review.dump(),
                                                        "Price, stock or mapping changed before supplier placement");
            if (!transition) { ++counters_.errors; log_.error("orders", transition.error()); }
            else log_.warning("orders", "Commande déplacée en révision après prévalidation fournisseur: " + stored.shopify_id);
            continue;
        }

        if (config_.app.dry_run || !config_.app.live_orders) {
''')
replace('src/elit21/orders/OrderManager.cpp',
'''            auto updated = db_.updateOrderSupplierId(stored.shopify_id, simulated_id, "supplier_ordered");
            if (!updated) {
''',
'''            Json transition_context = Json::object();
            transition_context.set("dry_run", true);
            transition_context.set("supplier_order_id", simulated_id);
            auto updated = db_.setSupplierOrderAndTransition(stored.shopify_id, "pending_supplier",
                                                             simulated_id, "supplier_ordered",
                                                             transition_context.dump());
            if (!updated || !updated.value()) {
''')
replace('src/elit21/orders/OrderManager.cpp',
'''                log_.error("orders", updated.error());
''',
'''                log_.error("orders", updated ? "Order state changed concurrently" : updated.error());
''',1)
replace('src/elit21/orders/OrderManager.cpp',
'''        auto updated = db_.updateOrderSupplierId(stored.shopify_id, placed.value().order_id, "supplier_ordered");
        if (!updated) {
''',
'''        Json transition_context = Json::object();
        transition_context.set("dry_run", false);
        transition_context.set("supplier_order_id", placed.value().order_id);
        transition_context.set("shipping_service", shipping_service);
        auto updated = db_.setSupplierOrderAndTransition(stored.shopify_id, "pending_supplier",
                                                         placed.value().order_id, "supplier_ordered",
                                                         transition_context.dump());
        if (!updated || !updated.value()) {
''')
replace('src/elit21/orders/OrderManager.cpp',
'''            log_.critical("orders", "Commande fournisseur créée, mais état local non mis à jour: " + updated.error());
''',
'''            log_.critical("orders", "Commande fournisseur créée, mais état local non mis à jour: " +
                                       (updated ? std::string("concurrent state change") : updated.error()));
''')

# Safe cents in the canonical PricingEngine.
write('src/elit21/pricing/PricingEngine.cpp',r'''#include "elit21/pricing/PricingEngine.h"

#include "elit21/shopify/ShopifyMoney.h"

#include <algorithm>
#include <cmath>

namespace elit21 {

Result<PriceBreakdown> PricingEngine::calculate(double supplier_cost_cad, double shipping_cad) const {
    if (!std::isfinite(supplier_cost_cad) || !std::isfinite(shipping_cad) ||
        supplier_cost_cad < 0.0 || shipping_cad < 0.0) {
        return Result<PriceBreakdown>::failure("Coût fournisseur ou livraison invalide.");
    }
    if (!std::isfinite(config_.markup_percent_before_shipping) ||
        config_.markup_percent_before_shipping < 100.0) {
        return Result<PriceBreakdown>::failure("La marge avant livraison doit être d'au moins 100%.");
    }
    auto cost = shopify::ShopifyMoney::fromDouble(supplier_cost_cad);
    auto shipping = shopify::ShopifyMoney::fromDouble(shipping_cad);
    if (!cost || !shipping) return Result<PriceBreakdown>::failure("Montant impossible à représenter en centimes.");
    auto final = shopify::ShopifyMoney::supplierPrice(
        cost.value(), config_.markup_percent_before_shipping, shipping.value());
    if (!final) return Result<PriceBreakdown>::failure(final.error());

    const auto markup_cents = static_cast<std::int64_t>(std::llround(
        static_cast<long double>(cost.value().cents()) *
        static_cast<long double>(config_.markup_percent_before_shipping) / 100.0L));
    const shopify::ShopifyMoney markup(markup_cents);
    shopify::ShopifyMoney before_shipping = cost.value() + markup;
    shopify::ShopifyMoney adjusted_final = final.value();
    const auto minimum = shopify::ShopifyMoney::fromDouble(std::max(0.0, config_.minimum_price_cad));
    const auto maximum = shopify::ShopifyMoney::fromDouble(config_.maximum_price_cad);
    if (!minimum || !maximum || maximum.value() <= minimum.value())
        return Result<PriceBreakdown>::failure("Bornes de prix invalides.");
    if (adjusted_final < minimum.value()) adjusted_final = minimum.value();
    if (adjusted_final > maximum.value())
        return Result<PriceBreakdown>::failure("Prix final supérieur au maximum configuré.");

    PriceBreakdown breakdown;
    breakdown.supplier_cost_cad = cost.value().toDouble();
    breakdown.markup_amount_cad = markup.toDouble();
    breakdown.gross_profit_before_shipping_cad = markup.toDouble();
    breakdown.price_before_shipping_cad = before_shipping.toDouble();
    breakdown.shipping_cad = shipping.value().toDouble();
    breakdown.final_price_cad = adjusted_final.toDouble();
    return Result<PriceBreakdown>::success(breakdown);
}

} // namespace elit21
''')

# Worker now records order state transitions rather than bypassing the state machine.
p=root/'apps/worker_service/main.cpp';s=p.read_text()
s=s.replace('''            auto updated = database.updateOrderStatus(order_id, "cancelled");
            if (!updated) return updated;
''','''            auto current = database.ordersByShopifyIds({order_id});
            if (!current) return elit21::Result<void>::failure(current.error());
            if (!current.value().empty()) {
                const auto status = current.value().front().status;
                auto updated = database.transitionOrderStatus(order_id, {status}, "cancelled",
                                                              "shopify_order_cancelled", raw.value().dump());
                if (!updated) return elit21::Result<void>::failure(updated.error());
            }
''')
s=s.replace('''            const auto order_id = raw.value().getScalarString("order_id");
            if (!order_id.empty()) database.updateOrderStatus(order_id, "refund_received");
''','''            const auto order_id = raw.value().getScalarString("order_id");
            if (!order_id.empty()) {
                auto current = database.ordersByShopifyIds({order_id});
                if (!current) return elit21::Result<void>::failure(current.error());
                if (!current.value().empty()) {
                    const auto status = current.value().front().status;
                    auto updated = database.transitionOrderStatus(order_id, {status}, "refund_received",
                                                                  "shopify_refund_created", raw.value().dump());
                    if (!updated) return elit21::Result<void>::failure(updated.error());
                }
            }
''')
p.write_text(s,encoding='utf-8')

# Extend unit test to prove stale stock is blocked at preflight through admission evaluation.
p=root/'tests/unit_tests.cpp';s=p.read_text()
s=s.replace('''        auto transitioned = database.transitionOrderStatus(
            "9007199254740993", {"pending_supplier"}, "supplier_ordered",
''','''        variant.stock = 2;
        expect(bool(database.upsertVariant(variant)), "stock fournisseur mis à jour avant prévalidation");
        auto stale_evaluation = admission.evaluate(mappedOrder("future-order"), true, true);
        expect(bool(stale_evaluation) && !stale_evaluation.value().accepted(),
               "stock égal au tampon de sécurité bloque la commande fournisseur");
        variant.stock = 10;
        expect(bool(database.upsertVariant(variant)), "stock fournisseur restauré");

        auto transitioned = database.transitionOrderStatus(
            "9007199254740993", {"pending_supplier"}, "supplier_ordered",
''')
p.write_text(s,encoding='utf-8')
print('phase6 applied')
