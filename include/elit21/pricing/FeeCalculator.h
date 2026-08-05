#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::pricing {

class FeeCalculator final : public platform::BusinessComponent {
public:
    FeeCalculator();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::pricing
