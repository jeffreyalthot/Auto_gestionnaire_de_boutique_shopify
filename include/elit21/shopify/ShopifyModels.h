#pragma once

#include "elit21/core/Result.h"
#include "elit21/core/Types.h"
#include "elit21/json/Json.h"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace elit21 {

struct ShopifyProductVariantCreate {
    std::string option_name{"Title"};
    std::string option_value;
    std::string sku;
    std::string source_product_id;
    std::string source_sku_id;
    double price_cad{0.0};
    double supplier_cost_cad{0.0};
    int inventory{0};
    bool taxable{true};
    [[nodiscard]] bool valid() const noexcept;
};

struct ShopifyProductCreate {
    std::string title;
    std::string description_html;
    std::string vendor{"ELIT21"};
    std::string product_type;
    double price_cad{0};
    double supplier_cost_cad{0};
    int inventory{0};
    std::string sku;
    std::string source_product_id;
    std::string source_sku_id;
    std::vector<std::string> image_urls;
    std::vector<ShopifyProductVariantCreate> variants;
    bool publish{false};
    [[nodiscard]] bool valid() const noexcept;
};

struct ShopifyVariantRef {
    std::string id;
    std::string inventory_item_id;
    std::string sku;
    std::string title;
    [[nodiscard]] bool valid() const noexcept;
};

struct ShopifyVariantPriceUpdate {
    std::string variant_id;
    double price_cad{0.0};
    [[nodiscard]] bool valid() const noexcept;
};

struct ShopifyInventoryQuantityUpdate {
    std::string inventory_item_id;
    int quantity{0};
    std::optional<int> compare_quantity;
    [[nodiscard]] bool valid() const noexcept;
    [[nodiscard]] bool compareAndSetEnabled() const noexcept { return compare_quantity.has_value(); }
};

struct ShopifyInventoryQuantitySnapshot {
    std::string inventory_item_id;
    std::string inventory_level_id;
    std::string updated_at;
    int available{0};
    [[nodiscard]] bool valid() const noexcept;
};

struct ShopifyFulfillmentOrderRef {
    std::string id;
    std::string status;
    [[nodiscard]] bool valid() const noexcept;
};

struct ShopifyOrderPage {
    std::vector<CustomerOrder> orders;
    std::string end_cursor;
    bool has_next_page{false};
    [[nodiscard]] bool cursorConsistent() const noexcept;
};

struct ShopifyWebhookSubscriptionRef {
    std::string id;
    std::string topic;
    std::string callback_url;
    [[nodiscard]] bool valid() const noexcept;
};

struct ShopifyBulkOperationRef {
    std::string id;
    std::string type;
    std::string status;
    std::string error_code;
    std::string url;
    std::string partial_data_url;
    std::uint64_t object_count{0};
    std::uint64_t root_object_count{0};
    [[nodiscard]] bool valid() const noexcept;
    [[nodiscard]] bool terminal() const noexcept;
    [[nodiscard]] bool successful() const noexcept;
};

struct ShopifyModelValidationIssue {
    std::size_t index{0};
    std::string field;
    std::string message;
    [[nodiscard]] Json toJson() const;
};

struct ShopifyProductValidationReport {
    bool valid{false};
    std::size_t variants_checked{0};
    std::size_t images_checked{0};
    std::vector<ShopifyModelValidationIssue> issues;
    [[nodiscard]] Json toJson() const;
};

class ShopifyModelValidator {
public:
    [[nodiscard]] static ShopifyProductValidationReport validateProduct(
        const ShopifyProductCreate& product,
        double minimum_markup_percent_before_shipping = 100.0);

    [[nodiscard]] static Result<void> validatePriceBatch(
        const std::vector<ShopifyVariantPriceUpdate>& updates,
        std::size_t maximum_batch_size = 250);

    [[nodiscard]] static Result<void> validateInventoryBatch(
        const std::vector<ShopifyInventoryQuantityUpdate>& updates,
        std::size_t maximum_batch_size = 250,
        bool require_compare_and_set = true);
};

} // namespace elit21
