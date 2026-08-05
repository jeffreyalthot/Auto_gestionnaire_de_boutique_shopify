#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::automation {

class AutomationRule final : public platform::BusinessComponent {
public:
    AutomationRule();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::automation
