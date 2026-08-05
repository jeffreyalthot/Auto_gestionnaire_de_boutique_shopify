#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::finance {

class OrderProfitabilityCalculator final : public platform::BusinessComponent {
public:
    OrderProfitabilityCalculator();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::finance
