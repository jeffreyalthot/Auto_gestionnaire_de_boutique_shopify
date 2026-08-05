#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::fulfillment {

class ShipmentStateMachine final : public platform::BusinessComponent {
public:
    ShipmentStateMachine();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::fulfillment
