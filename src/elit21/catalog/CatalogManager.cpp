#include "elit21/catalog/CatalogManager.h"

#include <algorithm>
#include <unordered_map>

namespace elit21 {
namespace {

Address freightEstimateAddress() {
    Address address;
    address.first_name = "ELIT21";
    address.last_name = "Estimate";
    address.address1 = "1 rue Notre-Dame";
    address.city = "Montreal";
    address.province = "Quebec";
    address.province_code = "QC";
    address.country = "Canada";
    address.country_code = "CA";
    address.postal_code = "H2Y 1C6";
    return address;
}

double cheapestEligibleShipping(AliExpressClient& client,
                                const AliProduct& product,
                                int maximum_days,
                                Logger& logger) {
    const auto sku = product.skus.empty() ? std::string("DEFAULT") : product.skus.front().sku_id;
    auto quotes = client.calculateFreight(product.product_id, sku, 1, freightEstimateAddress());
    if (!quotes) {
        logger.warning("catalog", "Transport non calculé pour " + product.product_id + ": " + quotes.error());
        return -1.0;
    }
    double best = -1.0;
    for (const auto& quote : quotes.value()) {
        if (!quote.available || quote.estimated_days <= 0 || quote.estimated_days > maximum_days) continue;
        if (best < 0.0 || quote.amount_cad < best) best = quote.amount_cad;
    }
    return best;
}

std::string externalSku(const std::string& product_id, const std::string& source_sku) {
    return "AE-" + product_id + "-" + (source_sku.empty() ? "DEFAULT" : source_sku);
}

} // namespace

void CatalogManager::synchronize() {
    auto recommended = ali_.recommendations(1, 25);
    if (!recommended) {
        log_.error("catalog", recommended.error());
        ++counters_.errors;
        return;
    }

    for (const auto& summary : recommended.value()) {
        ++counters_.products_scanned;
        auto detailed = ali_.getProduct(summary.product_id);
        if (!detailed) {
            ++counters_.products_rejected;
            log_.warning("catalog", summary.product_id + ": " + detailed.error());
            continue;
        }
        const auto& product = detailed.value();
        auto decision = sourcing_.evaluate(product, config_.sourcing.maximum_delivery_days);
        if (!decision.accepted) {
            ++counters_.products_rejected;
            log_.warning("sourcing", product.product_id + ": " + decision.reason);
            continue;
        }
        const double shipping = cheapestEligibleShipping(
            ali_, product, config_.sourcing.maximum_delivery_days, log_);
        if (shipping < 0.0) {
            ++counters_.products_rejected;
            log_.warning("sourcing", product.product_id + ": aucun transport Canada admissible");
            continue;
        }

        ++counters_.products_accepted;
        auto imported = importProduct(product, shipping);
        if (!imported) {
            ++counters_.errors;
            log_.error("catalog", imported.error());
        }
    }
}

Result<void> CatalogManager::importProduct(const AliProduct& product, double shipping) {
    if (!product.valid() || shipping < 0.0) {
        return Result<void>::failure("Produit AliExpress ou coût de livraison invalide.");
    }

    auto existing = db_.productByAliExpressId(product.product_id);
    if (!existing) return Result<void>::failure(existing.error());

    const double base_cost = product.min_price_cad > 0.0
        ? product.min_price_cad
        : (!product.skus.empty() ? product.skus.front().price_cad : 0.0);
    auto base_price = pricing_.calculate(base_cost, shipping);
    if (!base_price) return Result<void>::failure(base_price.error());

    StoredProduct stored;
    if (existing.value()) stored = *existing.value();
    stored.ae_id = product.product_id;
    stored.title = product.title;
    stored.cost = base_cost;
    stored.shipping = shipping;
    stored.price = base_price.value().final_price_cad;
    stored.stock = std::max(0, product.stock - config_.inventory.safety_buffer);
    stored.status = config_.sourcing.auto_publish ? "ready_to_publish" : "candidate";

    std::vector<ShopifyProductVariantCreate> desired_variants;
    if (product.skus.empty()) {
        ShopifyProductVariantCreate desired;
        desired.option_name = "Title";
        desired.option_value = "Default";
        desired.sku = externalSku(product.product_id, "DEFAULT");
        desired.source_product_id = product.product_id;
        desired.source_sku_id = "DEFAULT";
        desired.price_cad = stored.price;
        desired.supplier_cost_cad = base_cost;
        desired.inventory = stored.stock;
        desired_variants.push_back(std::move(desired));
    } else {
        desired_variants.reserve(product.skus.size());
        for (const auto& source : product.skus) {
            if (!source.valid()) continue;
            auto variant_price = pricing_.calculate(source.price_cad, shipping);
            if (!variant_price) return Result<void>::failure(variant_price.error());
            ShopifyProductVariantCreate desired;
            desired.option_name = "Title";
            desired.option_value = source.sku_code.empty() ? source.sku_id : source.sku_code;
            desired.sku = externalSku(product.product_id, source.sku_id);
            desired.source_product_id = product.product_id;
            desired.source_sku_id = source.sku_id;
            desired.price_cad = variant_price.value().final_price_cad;
            desired.supplier_cost_cad = source.price_cad;
            desired.inventory = std::max(0, source.stock - config_.inventory.safety_buffer);
            desired_variants.push_back(std::move(desired));
        }
    }
    if (desired_variants.empty()) return Result<void>::failure("Aucune variante AliExpress exploitable.");

    auto initial_save = db_.upsertProduct(stored);
    if (!initial_save) return initial_save;

    std::vector<ShopifyVariantRef> shopify_variants;
    const bool live_publication = config_.sourcing.auto_publish && !config_.app.dry_run;
    if (live_publication) {
        if (stored.shopify_id.empty()) {
            ShopifyProductCreate creation;
            creation.title = product.title;
            creation.description_html = product.description;
            creation.price_cad = desired_variants.front().price_cad;
            creation.supplier_cost_cad = desired_variants.front().supplier_cost_cad;
            creation.inventory = desired_variants.front().inventory;
            creation.source_product_id = product.product_id;
            creation.source_sku_id = desired_variants.front().source_sku_id;
            creation.sku = desired_variants.front().sku;
            creation.image_urls = product.images;
            creation.variants = desired_variants;
            creation.publish = false;

            auto created = shop_.createProduct(creation);
            if (!created) return Result<void>::failure(created.error());
            stored.shopify_id = created.value();
            stored.status = "shopify_created";
            auto saved = db_.upsertProduct(stored);
            if (!saved) return saved;
        }

        auto references = shop_.productVariants(stored.shopify_id);
        if (!references) return Result<void>::failure("Références Shopify non récupérées: " + references.error());
        shopify_variants = references.take();

        std::unordered_map<std::string, const ShopifyProductVariantCreate*> desired_by_sku;
        for (const auto& desired : desired_variants) desired_by_sku[desired.sku] = &desired;
        std::vector<ShopifyVariantPriceUpdate> price_updates;
        std::vector<ShopifyInventoryQuantityUpdate> inventory_updates;
        for (const auto& reference : shopify_variants) {
            const auto iterator = desired_by_sku.find(reference.sku);
            if (iterator == desired_by_sku.end()) continue;
            price_updates.push_back({reference.id, iterator->second->price_cad});
            ShopifyInventoryQuantityUpdate inventory_update;
            inventory_update.inventory_item_id = reference.inventory_item_id;
            inventory_update.quantity = iterator->second->inventory;
            inventory_updates.push_back(std::move(inventory_update));
        }
        if (!price_updates.empty()) {
            auto updated = shop_.updateVariantPrices(stored.shopify_id, price_updates);
            if (!updated) return Result<void>::failure("Mise à jour des prix Shopify: " + updated.error());
            counters_.price_updates += price_updates.size();
        }
        if (!inventory_updates.empty()) {
            auto location = shop_.primaryLocationId();
            if (!location) return Result<void>::failure("Emplacement Shopify: " + location.error());
            auto updated = shop_.updateInventories(location.value(), inventory_updates);
            if (!updated) return Result<void>::failure("Mise à jour des stocks Shopify: " + updated.error());
            counters_.inventory_updates += inventory_updates.size();
        }

        auto publication = shop_.primaryPublicationId();
        if (publication) {
            auto published = shop_.publishProduct(stored.shopify_id, publication.value());
            if (published) {
                stored.status = "published";
                ++counters_.products_published;
            } else {
                log_.warning("catalog", "Produit créé mais non publié: " + published.error());
            }
        } else {
            log_.warning("catalog", "Publication Shopify indisponible: " + publication.error());
        }
    }

    auto saved_product = db_.upsertProduct(stored);
    if (!saved_product) return saved_product;

    std::unordered_map<std::string, ShopifyVariantRef> shopify_by_sku;
    for (const auto& reference : shopify_variants) shopify_by_sku[reference.sku] = reference;

    for (const auto& desired : desired_variants) {
        StoredVariant variant;
        variant.ae_product_id = product.product_id;
        variant.ae_sku_id = desired.source_sku_id;
        variant.sku = desired.sku;
        variant.cost = desired.supplier_cost_cad;
        variant.shipping = shipping;
        variant.price = desired.price_cad;
        variant.stock = desired.inventory;
        variant.status = desired.inventory > 0 ? "active" : "out_of_stock";
        if (const auto iterator = shopify_by_sku.find(variant.sku); iterator != shopify_by_sku.end()) {
            variant.shopify_variant_id = iterator->second.id;
            variant.shopify_inventory_item_id = iterator->second.inventory_item_id;
        }
        auto variant_saved = db_.upsertVariant(variant);
        if (!variant_saved) return variant_saved;
        counters_.estimated_profit_cad += std::max(0.0, desired.price_cad - shipping - desired.supplier_cost_cad);
    }

    db_.audit("INFO", "catalog", existing.value() ? "Produit réconcilié" : "Produit importé",
              "{\"aliexpress_product_id\":\"" + product.product_id +
              "\",\"shopify_product_id\":\"" + stored.shopify_id +
              "\",\"variant_count\":" + std::to_string(desired_variants.size()) + "}");
    log_.info("pricing", product.product_id + " coût=" + std::to_string(stored.cost) +
                         " + marge100%=" + std::to_string(base_price.value().markup_amount_cad) +
                         " + livraison=" + std::to_string(shipping) + " => " +
                         std::to_string(stored.price) + " CAD");
    return Result<void>::success();
}

} // namespace elit21
