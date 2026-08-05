#pragma once

#include <string>
#include <vector>

namespace elit21 {

struct AliProductSku {
    std::string sku_id;
    std::string sku_code;
    double price_cad{0};
    int stock{0};
    [[nodiscard]] bool valid() const noexcept;
};

struct AliProduct {
    std::string product_id;
    std::string title;
    std::string description;
    std::string seller_id;
    std::string seller_name;
    double min_price_cad{0};
    double max_price_cad{0};
    double rating{0};
    double seller_score{0};
    int orders{0};
    int stock{0};
    std::vector<std::string> images;
    std::vector<AliProductSku> skus;
    [[nodiscard]] bool valid() const noexcept;
};

struct FreightQuote {
    std::string service_name;
    std::string service_code;
    double amount_cad{0};
    int estimated_days{0};
    bool available{false};
    bool tracking_available{false};
    [[nodiscard]] bool valid() const noexcept;
};

struct SupplierOrderResult {
    std::string order_id;
    std::vector<std::string> order_ids;
    std::string status;
    double amount{0};
    std::string raw_json;
    [[nodiscard]] bool valid() const noexcept;
};

} // namespace elit21
