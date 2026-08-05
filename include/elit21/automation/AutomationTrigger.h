#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::automation {

class AutomationTrigger final : public platform::BusinessComponent {
public:
    AutomationTrigger();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::automation
