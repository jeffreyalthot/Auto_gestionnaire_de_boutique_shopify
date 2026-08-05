#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::finance {

class CostOfGoodsCalculator final : public platform::BusinessComponent {
public:
    CostOfGoodsCalculator();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::finance
