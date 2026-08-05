#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::orders {

class OrderStateMachine final : public platform::BusinessComponent {
public:
    OrderStateMachine();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::orders
