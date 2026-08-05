#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::automation {

class AutomationEvent final : public platform::BusinessComponent {
public:
    AutomationEvent();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::automation
