#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::fulfillment {

class ShipmentMonitor final : public platform::BusinessComponent {
public:
    ShipmentMonitor();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::fulfillment
