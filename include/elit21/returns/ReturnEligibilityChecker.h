#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::returns {

class ReturnEligibilityChecker final : public platform::BusinessComponent {
public:
    ReturnEligibilityChecker();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::returns
