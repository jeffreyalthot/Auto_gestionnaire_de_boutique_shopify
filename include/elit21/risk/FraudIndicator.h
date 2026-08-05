#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::risk {

class FraudIndicator final : public platform::BusinessComponent {
public:
    FraudIndicator();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::risk
