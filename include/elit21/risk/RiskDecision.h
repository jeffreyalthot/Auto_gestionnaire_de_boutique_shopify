#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::risk {

class RiskDecision final : public platform::BusinessComponent {
public:
    RiskDecision();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::risk
