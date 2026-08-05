#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::fulfillment {

class FulfillmentCoordinator final : public platform::BusinessComponent {
public:
    FulfillmentCoordinator();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::fulfillment
