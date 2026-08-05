#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::automation {

class AutomationAction final : public platform::BusinessComponent {
public:
    AutomationAction();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::automation
