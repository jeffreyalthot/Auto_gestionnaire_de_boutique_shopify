#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::pricing {

class FreightCostCalculator final : public platform::BusinessComponent {
public:
    FreightCostCalculator();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::pricing
