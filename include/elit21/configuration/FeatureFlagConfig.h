#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::configuration {

class FeatureFlagConfig final : public platform::BusinessComponent {
public:
    FeatureFlagConfig();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::configuration
