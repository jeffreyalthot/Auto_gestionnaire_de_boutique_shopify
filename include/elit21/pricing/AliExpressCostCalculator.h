#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::pricing {

class AliExpressCostCalculator final : public platform::BusinessComponent {
public:
    AliExpressCostCalculator();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::pricing
