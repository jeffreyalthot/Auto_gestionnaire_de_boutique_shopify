#include "elit21/shopify/ShopifyModels.h"

#include "elit21/util/StringUtil.h"

#include <algorithm>
#include <cmath>
#include <set>
#include <sstream>

namespace elit21 {
namespace {

bool isHttpsUrl(const std::string& value) {
    return value.rfind("https://", 0) == 0 && value.size() > 8;
}

bool looksLikeGid(const std::string& value, const std::string& resource) {
    return value.rfind("gid://shopify/" + resource + "/", 0) == 0;
}

std::int64_t toCents(double amount) {
    return static_cast<std::int64_t>(std::llround(amount * 100.0));
}

void addIssue(ShopifyProductValidationReport& report,
              std::size_t index,
              std::string field,
              std::string message) {
    report.issues.push_back({index, std::move(field), std::move(message)});
}

} // namespace

bool ShopifyProductVariantCreate::valid() const noexcept {
    return !option_name.empty() && !option_value.empty() && !sku.empty() &&
           !source_product_id.empty() && !source_sku_id.empty() &&
           std::isfinite(price_cad) && price_cad >= 0.0 &&
           std::isfinite(supplier_cost_cad) && supplier_cost_cad >= 0.0 && inventory >= 0;
}

bool ShopifyProductCreate::valid() const noexcept {
    if (title.empty() || source_product_id.empty() || !std::isfinite(price_cad) || price_cad < 0.0 ||
        !std::isfinite(supplier_cost_cad) || supplier_cost_cad < 0.0 || inventory < 0) {
        return false;
    }
    if (variants.empty()) return !sku.empty() && !source_sku_id.empty();
    return std::all_of(variants.begin(), variants.end(), [](const auto& variant) { return variant.valid(); });
}

bool ShopifyVariantRef::valid() const noexcept {
    return looksLikeGid(id, "ProductVariant") && looksLikeGid(inventory_item_id, "InventoryItem");
}

bool ShopifyVariantPriceUpdate::valid() const noexcept {
    return looksLikeGid(variant_id, "ProductVariant") && std::isfinite(price_cad) && price_cad >= 0.0;
}

bool ShopifyInventoryQuantityUpdate::valid() const noexcept {
    return looksLikeGid(inventory_item_id, "InventoryItem") && quantity >= 0 &&
           (!compare_quantity.has_value() || *compare_quantity >= 0);
}

bool ShopifyInventoryQuantitySnapshot::valid() const noexcept {
    return looksLikeGid(inventory_item_id, "InventoryItem") &&
           looksLikeGid(inventory_level_id, "InventoryLevel") && available >= 0;
}

bool ShopifyFulfillmentOrderRef::valid() const noexcept {
    return looksLikeGid(id, "FulfillmentOrder") && !status.empty();
}

bool ShopifyOrderPage::cursorConsistent() const noexcept {
    return !has_next_page || !end_cursor.empty();
}

bool ShopifyWebhookSubscriptionRef::valid() const noexcept {
    return looksLikeGid(id, "WebhookSubscription") && !topic.empty() && isHttpsUrl(callback_url);
}

bool ShopifyBulkOperationRef::valid() const noexcept {
    return looksLikeGid(id, "BulkOperation") && !status.empty();
}

bool ShopifyBulkOperationRef::terminal() const noexcept {
    return status == "COMPLETED" || status == "FAILED" || status == "CANCELED" || status == "EXPIRED";
}

bool ShopifyBulkOperationRef::successful() const noexcept {
    return status == "COMPLETED" && !url.empty();
}

Json ShopifyModelValidationIssue::toJson() const {
    Json output = Json::object();
    output.set("index", static_cast<std::int64_t>(index));
    output.set("field", field);
    output.set("message", message);
    return output;
}

Json ShopifyProductValidationReport::toJson() const {
    Json output = Json::object();
    output.set("valid", valid);
    output.set("variants_checked", static_cast<std::int64_t>(variants_checked));
    output.set("images_checked", static_cast<std::int64_t>(images_checked));
    Json issue_array = Json::array();
    for (const auto& issue : issues) issue_array.push(issue.toJson());
    output.set("issues", issue_array);
    return output;
}

ShopifyProductValidationReport ShopifyModelValidator::validateProduct(
    const ShopifyProductCreate& product,
    double minimum_markup_percent_before_shipping) {
    ShopifyProductValidationReport report;
    report.variants_checked = product.variants.empty() ? 1U : product.variants.size();
    report.images_checked = product.image_urls.size();

    const auto title = util::trim(product.title);
    if (title.empty()) addIssue(report, 0, "title", "Product title is required");
    if (title.size() > 255) addIssue(report, 0, "title", "Product title exceeds 255 characters");
    if (util::trim(product.vendor).empty()) addIssue(report, 0, "vendor", "Vendor is required");
    if (util::trim(product.source_product_id).empty()) {
        addIssue(report, 0, "source_product_id", "AliExpress source product id is required");
    }
    if (!std::isfinite(minimum_markup_percent_before_shipping) ||
        minimum_markup_percent_before_shipping < 0.0) {
        addIssue(report, 0, "minimum_markup_percent_before_shipping", "Markup percentage is invalid");
    }

    for (std::size_t i = 0; i < product.image_urls.size(); ++i) {
        if (!isHttpsUrl(product.image_urls[i])) {
            addIssue(report, i, "image_urls", "Image URL must use HTTPS");
        }
    }

    std::set<std::string> skus;
    std::set<std::string> source_sku_ids;
    const auto validateCommercialLine = [&](std::size_t index,
                                            const std::string& sku,
                                            const std::string& source_sku_id,
                                            double supplier_cost,
                                            double sale_price,
                                            int inventory,
                                            const std::string& option_name,
                                            const std::string& option_value) {
        if (util::trim(sku).empty()) addIssue(report, index, "sku", "SKU is required");
        else if (!skus.insert(sku).second) addIssue(report, index, "sku", "Duplicate SKU in product payload");

        if (util::trim(source_sku_id).empty()) {
            addIssue(report, index, "source_sku_id", "AliExpress source SKU id is required");
        } else if (!source_sku_ids.insert(source_sku_id).second) {
            addIssue(report, index, "source_sku_id", "Duplicate AliExpress source SKU id");
        }

        if (!std::isfinite(supplier_cost) || supplier_cost < 0.0) {
            addIssue(report, index, "supplier_cost_cad", "Supplier cost must be finite and non-negative");
        }
        if (!std::isfinite(sale_price) || sale_price < 0.0) {
            addIssue(report, index, "price_cad", "Sale price must be finite and non-negative");
        }
        if (inventory < 0) addIssue(report, index, "inventory", "Inventory cannot be negative");
        if (util::trim(option_name).empty()) addIssue(report, index, "option_name", "Variant option name is required");
        if (util::trim(option_value).empty()) addIssue(report, index, "option_value", "Variant option value is required");

        if (std::isfinite(supplier_cost) && std::isfinite(sale_price) && supplier_cost >= 0.0 && sale_price >= 0.0) {
            const auto cost_cents = toCents(supplier_cost);
            const auto sale_cents = toCents(sale_price);
            const long double multiplier = 1.0L +
                static_cast<long double>(minimum_markup_percent_before_shipping) / 100.0L;
            const auto minimum_sale_cents = static_cast<std::int64_t>(std::ceil(cost_cents * multiplier));
            if (sale_cents < minimum_sale_cents) {
                std::ostringstream message;
                message << "Sale price violates the minimum "
                        << minimum_markup_percent_before_shipping
                        << "% markup before shipping";
                addIssue(report, index, "price_cad", message.str());
            }
        }
    };

    if (product.variants.empty()) {
        validateCommercialLine(0, product.sku, product.source_sku_id,
                               product.supplier_cost_cad, product.price_cad,
                               product.inventory, "Title", "Default Title");
    } else {
        for (std::size_t i = 0; i < product.variants.size(); ++i) {
            const auto& variant = product.variants[i];
            if (variant.source_product_id != product.source_product_id) {
                addIssue(report, i, "source_product_id", "Variant source product id differs from parent product");
            }
            validateCommercialLine(i, variant.sku, variant.source_sku_id,
                                   variant.supplier_cost_cad, variant.price_cad,
                                   variant.inventory, variant.option_name, variant.option_value);
        }
    }

    report.valid = report.issues.empty();
    return report;
}

Result<void> ShopifyModelValidator::validatePriceBatch(
    const std::vector<ShopifyVariantPriceUpdate>& updates,
    std::size_t maximum_batch_size) {
    if (updates.empty()) return Result<void>::failure("Price update batch cannot be empty");
    if (maximum_batch_size == 0 || updates.size() > maximum_batch_size) {
        return Result<void>::failure("Price update batch exceeds configured maximum size");
    }
    std::set<std::string> variant_ids;
    for (const auto& update : updates) {
        if (!update.valid()) return Result<void>::failure("Price update contains an invalid variant id or amount");
        if (!variant_ids.insert(update.variant_id).second) {
            return Result<void>::failure("Price update batch contains duplicate variant ids");
        }
    }
    return Result<void>::success();
}

Result<void> ShopifyModelValidator::validateInventoryBatch(
    const std::vector<ShopifyInventoryQuantityUpdate>& updates,
    std::size_t maximum_batch_size,
    bool require_compare_and_set) {
    if (updates.empty()) return Result<void>::failure("Inventory update batch cannot be empty");
    if (maximum_batch_size == 0 || updates.size() > maximum_batch_size) {
        return Result<void>::failure("Inventory update batch exceeds configured maximum size");
    }
    std::set<std::string> inventory_item_ids;
    for (const auto& update : updates) {
        if (!update.valid()) return Result<void>::failure("Inventory update contains invalid data");
        if (require_compare_and_set && !update.compareAndSetEnabled()) {
            return Result<void>::failure("Inventory compare-and-set value is required");
        }
        if (!inventory_item_ids.insert(update.inventory_item_id).second) {
            return Result<void>::failure("Inventory update batch contains duplicate inventory item ids");
        }
    }
    return Result<void>::success();
}

} // namespace elit21
