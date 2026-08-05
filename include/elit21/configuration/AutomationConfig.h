#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::configuration {

class AutomationConfig final : public platform::BusinessComponent {
public:
    AutomationConfig();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::configuration
