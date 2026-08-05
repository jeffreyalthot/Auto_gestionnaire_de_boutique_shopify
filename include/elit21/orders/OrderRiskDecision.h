#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::orders {

class OrderRiskDecision final : public platform::BusinessComponent {
public:
    OrderRiskDecision();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::orders
