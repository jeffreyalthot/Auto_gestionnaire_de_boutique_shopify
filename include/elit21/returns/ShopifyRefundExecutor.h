#pragma once

#include "elit21/core/Result.h"
#include "elit21/json/Json.h"
#include "elit21/platform/BusinessComponent.h"

#include <string>

namespace elit21::returns {

struct ShopifyRefundPlan {
    std::string shopify_order_id;
    std::string reason;
    double order_total_cad{0.0};
    double already_refunded_cad{0.0};
    double requested_refund_cad{0.0};
    double remaining_refundable_cad{0.0};
    bool restock{false};
    bool notify_customer{true};
    [[nodiscard]] Json toJson() const;
};

class ShopifyRefundExecutor final : public platform::BusinessComponent {
public:
    ShopifyRefundExecutor();
    static Result<ShopifyRefundPlan> plan(const std::string& shopify_order_id,
                                          double order_total_cad,
                                          double already_refunded_cad,
                                          double requested_refund_cad,
                                          std::string reason,
                                          bool restock,
                                          bool notify_customer);
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::returns
