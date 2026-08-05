#include "elit21/core/Types.h"

#include <algorithm>
#include <cmath>

namespace elit21 {

bool Money::valid() const noexcept {
    return std::isfinite(amount) && amount >= 0.0 && currency == "CAD";
}

bool Address::isCanadian() const noexcept {
    return country_code == "CA" || country == "Canada" || country == "CANADA";
}

bool Address::hasRequiredShippingFields() const noexcept {
    return isCanadian() && !first_name.empty() && !last_name.empty() && !address1.empty() &&
           !city.empty() && (!province_code.empty() || !province.empty()) && !postal_code.empty();
}

bool OrderLine::valid() const noexcept {
    return quantity > 0 && std::isfinite(unit_price_cad) && unit_price_cad >= 0.0 &&
           (!sku.empty() || (!aliexpress_product_id.empty() && !aliexpress_sku_id.empty()));
}

bool CustomerOrder::valid() const noexcept {
    return !shopify_order_id.empty() && currency == "CAD" && std::isfinite(total_cad) && total_cad > 0.0 &&
           shipping_address.hasRequiredShippingFields() && !lines.empty() &&
           std::all_of(lines.begin(), lines.end(), [](const OrderLine& line) { return line.valid(); });
}

bool ShipmentInfo::trackable() const noexcept {
    return !tracking_number.empty() && !carrier.empty();
}

} // namespace elit21
