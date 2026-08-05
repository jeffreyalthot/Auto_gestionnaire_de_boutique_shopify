#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::fulfillment {

class PartialFulfillmentCoordinator final : public platform::BusinessComponent {
public:
    PartialFulfillmentCoordinator();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::fulfillment
