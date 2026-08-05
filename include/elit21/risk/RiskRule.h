#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::risk {

class RiskRule final : public platform::BusinessComponent {
public:
    RiskRule();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::risk
