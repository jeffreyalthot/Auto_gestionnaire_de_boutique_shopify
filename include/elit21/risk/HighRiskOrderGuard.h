#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::risk {

class HighRiskOrderGuard final : public platform::BusinessComponent {
public:
    HighRiskOrderGuard();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::risk
