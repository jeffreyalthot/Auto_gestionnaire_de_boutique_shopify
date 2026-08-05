#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::finance {

class GrossProfitCalculator final : public platform::BusinessComponent {
public:
    GrossProfitCalculator();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::finance
