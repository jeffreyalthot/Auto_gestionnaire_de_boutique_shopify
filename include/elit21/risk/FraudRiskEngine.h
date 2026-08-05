#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::risk {

class FraudRiskEngine final : public platform::BusinessComponent {
public:
    FraudRiskEngine();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::risk
