#include "elit21/aliexpress/AliExpressModels.h"

#include <algorithm>
#include <cmath>

namespace elit21 {

bool AliProductSku::valid() const noexcept {
    return !sku_id.empty() && std::isfinite(price_cad) && price_cad >= 0.0 && stock >= 0;
}

bool AliProduct::valid() const noexcept {
    return !product_id.empty() && !title.empty() && std::isfinite(min_price_cad) && min_price_cad >= 0.0 &&
           std::isfinite(max_price_cad) && max_price_cad >= min_price_cad && stock >= 0 &&
           std::all_of(skus.begin(), skus.end(), [](const AliProductSku& sku) { return sku.valid(); });
}

bool FreightQuote::valid() const noexcept {
    return !service_code.empty() && std::isfinite(amount_cad) && amount_cad >= 0.0 && estimated_days >= 0;
}

bool SupplierOrderResult::valid() const noexcept {
    return (!order_id.empty() || !order_ids.empty()) && !status.empty() && std::isfinite(amount) && amount >= 0.0;
}

} // namespace elit21
