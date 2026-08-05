#pragma once

#include "elit21/core/Result.h"
#include "elit21/json/Json.h"
#include "elit21/platform/BusinessComponent.h"

namespace elit21::pricing {

struct ShopifyFeePolicy {
    double payment_percentage{0.0};
    double payment_fixed_cad{0.0};
    double platform_transaction_percentage{0.0};
    double reserve_percentage{0.0};
    [[nodiscard]] Result<void> validate() const;
};

struct ShopifyFeeEstimate {
    double gross_cad{0.0};
    double payment_fee_cad{0.0};
    double platform_fee_cad{0.0};
    double reserve_cad{0.0};
    double total_fees_cad{0.0};
    double net_after_fees_cad{0.0};
    [[nodiscard]] Json toJson() const;
};

class ShopifyFeeEstimator final : public platform::BusinessComponent {
public:
    ShopifyFeeEstimator();
    static Result<ShopifyFeeEstimate> estimate(double gross_cad, const ShopifyFeePolicy& policy);
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::pricing
