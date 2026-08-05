#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace elit21 {

struct Money {
    double amount{0.0};
    std::string currency{"CAD"};
    [[nodiscard]] bool valid() const noexcept;
};

struct Address {
    std::string first_name, last_name, company, address1, address2, city, province, province_code;
    std::string country{"Canada"}, country_code{"CA"}, postal_code, phone;
    [[nodiscard]] bool isCanadian() const noexcept;
    [[nodiscard]] bool hasRequiredShippingFields() const noexcept;
};

struct OrderLine {
    std::string shopify_line_id, shopify_variant_id, aliexpress_product_id, aliexpress_sku_id, sku, title;
    int quantity{0};
    double unit_price_cad{0.0};
    [[nodiscard]] bool valid() const noexcept;
};

struct CustomerOrder {
    std::string shopify_order_id, order_name, email, phone, currency{"CAD"};
    Address shipping_address;
    std::vector<OrderLine> lines;
    double total_cad{0.0};
    std::string raw_json;
    [[nodiscard]] bool valid() const noexcept;
};

struct ShipmentInfo {
    std::string tracking_number, carrier, status, last_event;
    [[nodiscard]] bool trackable() const noexcept;
};

struct RuntimeCounters {
    std::uint64_t products_scanned{0}, products_accepted{0}, products_rejected{0}, products_published{0};
    std::uint64_t inventory_updates{0}, price_updates{0}, orders_received{0}, supplier_orders{0};
    std::uint64_t shipped{0}, delivered{0}, accepted_events{0}, rejected_events{0}, errors{0};
    double estimated_profit_cad{0.0};
};

} // namespace elit21
