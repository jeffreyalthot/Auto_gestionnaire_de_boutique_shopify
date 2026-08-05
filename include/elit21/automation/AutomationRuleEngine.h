#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::automation {

class AutomationRuleEngine final : public platform::BusinessComponent {
public:
    AutomationRuleEngine();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::automation
