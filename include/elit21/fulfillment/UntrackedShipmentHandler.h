#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::fulfillment {

class UntrackedShipmentHandler final : public platform::BusinessComponent {
public:
    UntrackedShipmentHandler();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::fulfillment
