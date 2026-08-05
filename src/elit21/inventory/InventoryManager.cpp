#include "elit21/inventory/InventoryManager.h"
#include "elit21/shopify/services/ShopifyInventoryService.h"

#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <vector>

namespace elit21 {
namespace {

struct PlannedVariantSync {
    StoredVariant variant;
    std::string product_gid;
    bool stock_changed{false};
    bool price_changed{false};
};

} // namespace

void InventoryManager::synchronize() {
    auto stored_variants = db_.variants(config_.inventory.sync_batch_size);
    if (!stored_variants) {
        log_.error("inventory", stored_variants.error());
        ++counters_.errors;
        return;
    }
    if (stored_variants.value().empty()) return;

    std::unordered_map<std::string, AliProduct> product_cache;
    std::unordered_map<std::string, std::string> shopify_product_ids;
    std::vector<PlannedVariantSync> plans;
    plans.reserve(stored_variants.value().size());

    for (auto variant : stored_variants.value()) {
        auto cache = product_cache.find(variant.ae_product_id);
        if (cache == product_cache.end()) {
            auto fetched = ali_.getProduct(variant.ae_product_id);
            if (!fetched) {
                ++counters_.errors;
                log_.warning("inventory", variant.ae_product_id + ": " + fetched.error());
                continue;
            }
            cache = product_cache.emplace(variant.ae_product_id, fetched.take()).first;
        }

        const AliProductSku* source = nullptr;
        for (const auto& candidate : cache->second.skus) {
            if (candidate.sku_id == variant.ae_sku_id) {
                source = &candidate;
                break;
            }
        }
        const int supplier_stock = source != nullptr ? source->stock : cache->second.stock;
        const double supplier_cost = source != nullptr && source->price_cad > 0.0
            ? source->price_cad : cache->second.min_price_cad;
        const int safe_stock = std::max(config_.inventory.out_of_stock_quantity,
                                        supplier_stock - config_.inventory.safety_buffer);
        auto calculated = pricing_.calculate(supplier_cost, variant.shipping);
        if (!calculated) {
            ++counters_.errors;
            log_.error("inventory", calculated.error());
            continue;
        }

        PlannedVariantSync plan;
        plan.stock_changed = safe_stock != variant.stock;
        plan.price_changed = std::fabs(calculated.value().final_price_cad - variant.price) > 0.009;
        variant.stock = safe_stock;
        variant.cost = supplier_cost;
        variant.price = calculated.value().final_price_cad;
        variant.status = safe_stock > 0 ? "active" : "out_of_stock";
        plan.variant = std::move(variant);

        auto product_id = shopify_product_ids.find(plan.variant.ae_product_id);
        if (product_id == shopify_product_ids.end()) {
            auto stored_product = db_.productByAliExpressId(plan.variant.ae_product_id);
            if (!stored_product) {
                ++counters_.errors;
                log_.warning("inventory", stored_product.error());
            } else {
                product_id = shopify_product_ids.emplace(
                    plan.variant.ae_product_id,
                    stored_product.value() ? stored_product.value()->shopify_id : std::string{}).first;
            }
        }
        if (product_id != shopify_product_ids.end()) plan.product_gid = product_id->second;
        plans.push_back(std::move(plan));
    }

    const bool shopify_ready = !config_.app.dry_run && !config_.shopify.access_token.empty();
    std::unordered_map<std::string, bool> inventory_item_succeeded;
    std::unordered_map<std::string, bool> price_batch_succeeded;

    if (shopify_ready) {
        std::vector<ShopifyInventoryQuantityUpdate> compare_and_set_updates;
        std::vector<ShopifyInventoryQuantityUpdate> bootstrap_updates;
        for (const auto& plan : plans) {
            if (!plan.stock_changed || plan.variant.shopify_inventory_item_id.empty()) continue;
            ShopifyInventoryQuantityUpdate update;
            update.inventory_item_id = plan.variant.shopify_inventory_item_id;
            update.quantity = plan.variant.stock;
            if (plan.variant.shopify_quantity >= 0) {
                update.compare_quantity = plan.variant.shopify_quantity;
                compare_and_set_updates.push_back(update);
            } else {
                bootstrap_updates.push_back(update);
            }
            inventory_item_succeeded[update.inventory_item_id] = false;
        }

        if (!compare_and_set_updates.empty() || !bootstrap_updates.empty()) {
            auto location = shop_.primaryLocationId();
            if (!location) {
                ++counters_.errors;
                log_.warning("inventory", "Emplacement Shopify indisponible: " + location.error());
            } else {
                shopify::services::ShopifyInventoryService service(shop_, db_);

                // Before an unchecked bootstrap, discover the actual Shopify quantity whenever
                // the inventory level already exists. Those items can then use compare-and-set.
                if (!bootstrap_updates.empty()) {
                    std::vector<std::string> bootstrap_ids;
                    bootstrap_ids.reserve(bootstrap_updates.size());
                    for (const auto& update : bootstrap_updates) {
                        bootstrap_ids.push_back(update.inventory_item_id);
                    }
                    auto snapshots = shop_.inventoryQuantitiesAtLocation(
                        location.value(), bootstrap_ids);
                    if (snapshots) {
                        std::unordered_map<std::string, int> current;
                        for (const auto& snapshot : snapshots.value()) {
                            current[snapshot.inventory_item_id] = snapshot.available;
                            db_.updateVariantShopifyQuantity(
                                snapshot.inventory_item_id, snapshot.available);
                        }
                        std::vector<ShopifyInventoryQuantityUpdate> still_unchecked;
                        for (auto update : bootstrap_updates) {
                            const auto found = current.find(update.inventory_item_id);
                            if (found == current.end()) {
                                still_unchecked.push_back(std::move(update));
                            } else {
                                update.compare_quantity = found->second;
                                compare_and_set_updates.push_back(std::move(update));
                            }
                        }
                        bootstrap_updates = std::move(still_unchecked);
                    } else {
                        log_.warning("inventory",
                            "Lecture des quantités Shopify avant amorçage impossible: " +
                            snapshots.error());
                    }
                }

                const auto persist_success = [&](
                    const std::vector<ShopifyInventoryQuantityUpdate>& updates) {
                    for (const auto& update : updates) {
                        inventory_item_succeeded[update.inventory_item_id] = true;
                        auto persisted = db_.updateVariantShopifyQuantity(
                            update.inventory_item_id, update.quantity);
                        if (!persisted) {
                            ++counters_.errors;
                            log_.warning("inventory",
                                "Quantité Shopify écrite mais checkpoint local non persisté pour " +
                                update.inventory_item_id + ": " + persisted.error());
                        }
                    }
                    counters_.inventory_updates += updates.size();
                };

                const auto apply_batch = [&](
                    const std::vector<ShopifyInventoryQuantityUpdate>& updates,
                    const std::string& business_key,
                    bool allow_unchecked) -> Result<void> {
                    if (updates.empty()) return Result<void>::success();
                    const auto result = service.setQuantitiesSafely(
                        location.value(), updates, business_key,
                        "elit21://inventory/synchronization", allow_unchecked);
                    if (!result) {
                        for (const auto& update : updates) {
                            inventory_item_succeeded[update.inventory_item_id] = false;
                        }
                        return result;
                    }
                    persist_success(updates);
                    return Result<void>::success();
                };

                auto compared = apply_batch(compare_and_set_updates,
                    "inventory-sync-cas:" + location.value(), false);
                if (!compared && !compare_and_set_updates.empty()) {
                    // A compare failure means the local checkpoint can be stale. Refresh the
                    // remote values once and retry with a new idempotent request.
                    std::vector<std::string> item_ids;
                    item_ids.reserve(compare_and_set_updates.size());
                    for (const auto& update : compare_and_set_updates) {
                        item_ids.push_back(update.inventory_item_id);
                    }
                    auto snapshots = shop_.inventoryQuantitiesAtLocation(
                        location.value(), item_ids);
                    if (snapshots) {
                        std::unordered_map<std::string, int> current;
                        for (const auto& snapshot : snapshots.value()) {
                            current[snapshot.inventory_item_id] = snapshot.available;
                            db_.updateVariantShopifyQuantity(
                                snapshot.inventory_item_id, snapshot.available);
                        }
                        std::vector<ShopifyInventoryQuantityUpdate> refreshed;
                        for (auto update : compare_and_set_updates) {
                            const auto found = current.find(update.inventory_item_id);
                            if (found != current.end()) {
                                update.compare_quantity = found->second;
                                refreshed.push_back(std::move(update));
                            }
                        }
                        auto retried = apply_batch(refreshed,
                            "inventory-sync-cas-refreshed:" + location.value(), false);
                        if (!retried) {
                            ++counters_.errors;
                            log_.warning("inventory",
                                "Lot Shopify CAS rejeté après réconciliation: " + retried.error());
                        }
                    } else {
                        ++counters_.errors;
                        log_.warning("inventory",
                            "Lot Shopify CAS rejeté et quantité distante illisible: " +
                            compared.error() + "; " + snapshots.error());
                    }
                }

                auto bootstrapped = apply_batch(bootstrap_updates,
                    "inventory-sync-bootstrap:" + location.value(), true);
                if (!bootstrapped) {
                    ++counters_.errors;
                    log_.warning("inventory",
                        "Lot Shopify d'amorçage rejeté: " + bootstrapped.error());
                }
            }
        }

        std::unordered_map<std::string, std::vector<ShopifyVariantPriceUpdate>> prices_by_product;
        for (const auto& plan : plans) {
            if (plan.price_changed && !plan.product_gid.empty() && !plan.variant.shopify_variant_id.empty()) {
                prices_by_product[plan.product_gid].push_back(
                    {plan.variant.shopify_variant_id, plan.variant.price});
            }
        }
        for (auto& [product_gid, updates] : prices_by_product) {
            auto updated = shop_.updateVariantPrices(product_gid, updates);
            price_batch_succeeded[product_gid] = static_cast<bool>(updated);
            if (!updated) {
                ++counters_.errors;
                log_.warning("pricing", "Lot de prix Shopify rejeté pour " + product_gid + ": " + updated.error());
            } else {
                counters_.price_updates += updates.size();
            }
        }
    }

    for (auto& plan : plans) {
        if (shopify_ready && plan.stock_changed && !plan.variant.shopify_inventory_item_id.empty()) {
            const auto item_status = inventory_item_succeeded.find(plan.variant.shopify_inventory_item_id);
            if (item_status != inventory_item_succeeded.end() && !item_status->second) {
                plan.variant.status = "inventory_sync_error";
            } else if (item_status != inventory_item_succeeded.end() && item_status->second) {
                plan.variant.shopify_quantity = plan.variant.stock;
            }
        }
        if (shopify_ready && plan.price_changed && !plan.product_gid.empty()) {
            const auto status = price_batch_succeeded.find(plan.product_gid);
            if (status != price_batch_succeeded.end() && !status->second) {
                plan.variant.status = "price_sync_error";
            }
        }
        auto saved = db_.upsertVariant(plan.variant);
        if (!saved) {
            ++counters_.errors;
            log_.error("inventory", saved.error());
            continue;
        }
        if (!shopify_ready && plan.stock_changed) ++counters_.inventory_updates;
        if (!shopify_ready && plan.price_changed) ++counters_.price_updates;
        db_.metric("inventory.available", static_cast<double>(plan.variant.stock),
                   "{\"sku\":\"" + plan.variant.sku + "\"}");
        db_.metric("pricing.sale_price_cad", plan.variant.price,
                   "{\"sku\":\"" + plan.variant.sku + "\"}");
    }
}

} // namespace elit21
