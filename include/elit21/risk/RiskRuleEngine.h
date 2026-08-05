#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::risk {

class RiskRuleEngine final : public platform::BusinessComponent {
public:
    RiskRuleEngine();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::risk
