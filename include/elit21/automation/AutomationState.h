#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::automation {

class AutomationState final : public platform::BusinessComponent {
public:
    AutomationState();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::automation
