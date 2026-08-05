#include "elit21/orders/OrderManager.h"

#include "elit21/json/Json.h"
#include "elit21/security/Crypto.h"

#include <algorithm>
#include <limits>

namespace elit21 {

void OrderManager::pollShopify() {
    constexpr int page_size = 250;
    constexpr int maximum_pages = 20;
    std::string cursor;
    int page_index = 0;
    int scanned = 0;
    int accepted = 0;
    int rejected = 0;

    auto checkpoint = db_.syncCheckpoint("shopify.paid_unfulfilled_orders");
    if (checkpoint && checkpoint.value()) {
        auto parsed = Json::parse(*checkpoint.value());
        if (parsed && !parsed.value().getBool("completed", true)) {
            cursor = parsed.value().getString("cursor");
            log_.info("orders", "Reprise Shopify depuis le checkpoint de pagination précédent.");
        }
    }

    for (; page_index < maximum_pages; ++page_index) {
        auto page = shop_.fetchOpenPaidOrdersPage(page_size, cursor);
        if (!page) {
            Json metadata = Json::object();
            metadata.set("page", page_index);
            metadata.set("scanned", scanned);
            metadata.set("accepted", accepted);
            metadata.set("rejected", rejected);
            metadata.set("error", page.error());
            db_.setSyncCheckpoint("shopify.paid_unfulfilled_orders", cursor, metadata.dump(), false);
            db_.metric("shopify.orders.poll.error", 1.0, metadata.dump());
            log_.error("orders", page.error());
            ++counters_.errors;
            return;
        }

        for (auto& order : page.value().orders) {
            ++scanned;
            auto ingested = ingest(std::move(order));
            if (!ingested) {
                ++rejected;
                log_.warning("orders", ingested.error());
            } else {
                ++accepted;
            }
        }

        Json metadata = Json::object();
        metadata.set("page", page_index + 1);
        metadata.set("scanned", scanned);
        metadata.set("accepted", accepted);
        metadata.set("rejected", rejected);
        metadata.set("has_next_page", page.value().has_next_page);
        db_.setSyncCheckpoint("shopify.paid_unfulfilled_orders",
                              page.value().end_cursor,
                              metadata.dump(),
                              !page.value().has_next_page);

        if (!page.value().has_next_page) break;
        if (page.value().end_cursor.empty() || page.value().end_cursor == cursor) {
            ++counters_.errors;
            db_.audit("ERROR", "shopify", "Order pagination cursor did not advance", metadata.dump());
            return;
        }
        cursor = page.value().end_cursor;
    }

    Json metrics = Json::object();
    metrics.set("pages", page_index + 1);
    metrics.set("scanned", scanned);
    metrics.set("accepted", accepted);
    metrics.set("rejected", rejected);
    db_.metric("shopify.orders.poll.scanned", static_cast<double>(scanned), metrics.dump());
    db_.audit("INFO", "shopify", "Paid order reconciliation completed", metrics.dump());
}

Result<void> OrderManager::resolveMappings(CustomerOrder& order) {
    if (order.lines.empty()) return Result<void>::failure("Commande sans ligne.");
    for (auto& line : order.lines) {
        if (line.quantity <= 0) return Result<void>::failure("Quantité invalide pour " + line.sku);
        if (!line.aliexpress_product_id.empty() && !line.aliexpress_sku_id.empty()) continue;
        if (line.sku.rfind("AE-", 0) == 0) {
            const auto separator = line.sku.find('-', 3);
            if (separator != std::string::npos && separator + 1 < line.sku.size()) {
                line.aliexpress_product_id = line.sku.substr(3, separator - 3);
                line.aliexpress_sku_id = line.sku.substr(separator + 1);
            }
        }
        if (line.aliexpress_product_id.empty() || line.aliexpress_sku_id.empty()) {
            log_.warning("orders", "SKU sans mappage AliExpress; révision manuelle requise: " + line.sku);
        }
    }
    return Result<void>::success();
}

Result<void> OrderManager::ingest(CustomerOrder order) {
    if (order.shopify_order_id.empty()) return Result<void>::failure("Identifiant Shopify manquant.");
    if (order.currency != "CAD") return Result<void>::failure("Commande non CAD rejetée: " + order.currency);
    if (order.total_cad <= 0.0) return Result<void>::failure("Total de commande invalide.");
    auto mappings = resolveMappings(order); if (!mappings) return mappings;
    if (config_.risk.require_valid_canadian_postal_code) {auto compliance=compliance_.validateAddress(order.shipping_address,config_.risk.block_po_boxes);if(!compliance){++counters_.rejected_events;return compliance;}}
    auto approved=risk_.approveOrder(order);if(!approved){++counters_.rejected_events;return approved;}
    auto admitted=admission_.admit(order,config_.app.dry_run||!config_.app.live_orders,true);if(!admitted){++counters_.rejected_events;return Result<void>::failure(admitted.error());}
    switch(admitted.value().decision){case shopify::ShopifyOrderAdmissionDecision::accepted:++counters_.orders_received;++counters_.accepted_events;counters_.estimated_profit_cad+=admitted.value().estimated_profit_cad;log_.info("orders","Commande Shopify "+order.shopify_order_id+" admise et mise en file fournisseur.");return Result<void>::success();case shopify::ShopifyOrderAdmissionDecision::duplicate:log_.debug("orders","Doublon Shopify ignoré: "+order.shopify_order_id);return Result<void>::success();case shopify::ShopifyOrderAdmissionDecision::manual_review:++counters_.orders_received;++counters_.accepted_events;log_.warning("orders","Commande Shopify "+order.shopify_order_id+" placée en révision manuelle.");return Result<void>::success();default:++counters_.rejected_events;return Result<void>::failure("Commande Shopify rejetée par le service d'admission.");}
}

std::string OrderManager::chooseShipping(const CustomerOrder& order) {
    std::string selected;
    int slowest_days = 0;
    double total_cost = 0.0;
    for (const auto& line : order.lines) {
        auto freight = ali_.calculateFreight(line.aliexpress_product_id,
                                             line.aliexpress_sku_id,
                                             line.quantity,
                                             order.shipping_address);
        if (!freight) return {};
        const FreightQuote* best = nullptr;
        for (const auto& quote : freight.value()) {
            if (!quote.available || quote.estimated_days > config_.sourcing.maximum_delivery_days) continue;
            if (best == nullptr || quote.amount_cad < best->amount_cad ||
                (quote.amount_cad == best->amount_cad && quote.estimated_days < best->estimated_days)) {
                best = &quote;
            }
        }
        if (best == nullptr) return {};
        if (selected.empty()) selected = best->service_code;
        if (selected != best->service_code) selected = "CAINIAO_STANDARD";
        slowest_days = std::max(slowest_days, best->estimated_days);
        total_cost += best->amount_cad;
    }
    log_.debug("orders", "Transport sélectionné=" + selected + " coût estimé=" +
                             std::to_string(total_cost) + " CAD, délai max=" +
                             std::to_string(slowest_days) + " jours");
    return selected;
}

void OrderManager::processPending() {
    auto pending = db_.ordersByStatus("pending_supplier", 50);
    if (!pending) {
        log_.error("orders", pending.error());
        ++counters_.errors;
        return;
    }

    for (const auto& stored : pending.value()) {
        auto parsed = Json::parse(stored.payload);
        if (!parsed) {
            ++counters_.errors;
            log_.error("orders", "Payload de commande invalide: " + parsed.error());
            continue;
        }

        CustomerOrder order;
        const auto root = parsed.value();
        order.shopify_order_id = stored.shopify_id;
        order.order_name = root.getString("name");
        order.email = root.getString("email");
        order.phone = root.getString("phone");
        order.currency = root.getString("currency", "CAD");
        order.total_cad = stored.total;
        order.raw_json = stored.payload;
        const auto address = root.get("shipping_address");
        order.shipping_address.first_name = address.getString("first_name");
        order.shipping_address.last_name = address.getString("last_name");
        order.shipping_address.company = address.getString("company");
        order.shipping_address.address1 = address.getString("address1");
        order.shipping_address.address2 = address.getString("address2");
        order.shipping_address.city = address.getString("city");
        order.shipping_address.province = address.getString("province");
        order.shipping_address.province_code = address.getString("province_code");
        order.shipping_address.country_code = address.getString("country_code", "CA");
        order.shipping_address.postal_code = address.getString("zip");
        order.shipping_address.phone = address.getString("phone");

        const auto lines = root.get("line_items");
        for (std::size_t index = 0; index < lines.size(); ++index) {
            const auto value = lines.at(index);
            OrderLine line;
            line.shopify_line_id = value.getString("shopify_line_id");
            line.shopify_variant_id = value.getString("shopify_variant_id");
            line.sku = value.getString("sku");
            line.quantity = value.getInt("quantity");
            line.title = value.getString("title");
            line.unit_price_cad = value.getNumber("unit_price_cad");
            line.aliexpress_product_id = value.getString("aliexpress_product_id");
            line.aliexpress_sku_id = value.getString("aliexpress_sku_id");
            order.lines.push_back(std::move(line));
        }

        auto mapping = resolveMappings(order);
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
            const std::string simulated_id = "DRYRUN-" + stored.shopify_id;
            Json transition_context = Json::object();
            transition_context.set("dry_run", true);
            transition_context.set("supplier_order_id", simulated_id);
            auto updated = db_.setSupplierOrderAndTransition(stored.shopify_id, "pending_supplier",
                                                             simulated_id, "supplier_ordered",
                                                             transition_context.dump());
            if (!updated || !updated.value()) {
                ++counters_.errors;
                log_.error("orders", updated ? "Order state changed concurrently" : updated.error());
                continue;
            }
            ++counters_.supplier_orders;
            db_.audit("INFO", "orders", "Commande fournisseur simulée", "{}", stored.shopify_id);
            log_.info("orders", "DRY RUN: commande fournisseur simulée " + simulated_id);
            continue;
        }

        const auto shipping_service = chooseShipping(order);
        if (shipping_service.empty()) {
            ++counters_.errors;
            log_.error("orders", "Aucun transport admissible vers le Canada pour " + stored.shopify_id);
            continue;
        }

        auto placed = ali_.placeOrder(order, shipping_service);
        if (!placed) {
            ++counters_.errors;
            db_.audit("ERROR", "orders", "Échec commande AliExpress", "{\"error\":\"" + placed.error() + "\"}", stored.shopify_id);
            log_.error("orders", placed.error());
            continue;
        }
        Json transition_context = Json::object();
        transition_context.set("dry_run", false);
        transition_context.set("supplier_order_id", placed.value().order_id);
        transition_context.set("shipping_service", shipping_service);
        auto updated = db_.setSupplierOrderAndTransition(stored.shopify_id, "pending_supplier",
                                                         placed.value().order_id, "supplier_ordered",
                                                         transition_context.dump());
        if (!updated || !updated.value()) {
            ++counters_.errors;
            log_.critical("orders", "Commande fournisseur créée, mais état local non mis à jour: " +
                                       (updated ? std::string("concurrent state change") : updated.error()));
            continue;
        }
        ++counters_.supplier_orders;
        db_.audit("INFO", "orders", "Commande AliExpress créée", placed.value().raw_json, stored.shopify_id);
        log_.info("orders", "Commande AliExpress créée: " + placed.value().order_id);
    }
}

} // namespace elit21
