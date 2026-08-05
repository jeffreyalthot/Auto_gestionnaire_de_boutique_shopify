#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::finance {

class PaymentFeeReconciler final : public platform::BusinessComponent {
public:
    PaymentFeeReconciler();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::finance
