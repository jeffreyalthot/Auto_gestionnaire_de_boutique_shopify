#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::fulfillment {

class FulfillmentCreationService final : public platform::BusinessComponent {
public:
    FulfillmentCreationService();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::fulfillment
