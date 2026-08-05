#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::sourcing {

class ShippingEligibilityAnalyzer final : public platform::BusinessComponent {
public:
    ShippingEligibilityAnalyzer();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::sourcing
