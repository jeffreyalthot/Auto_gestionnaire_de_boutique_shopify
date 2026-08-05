#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::pricing {

class MarginCalculator final : public platform::BusinessComponent {
public:
    MarginCalculator();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::pricing
