#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::fulfillment {

class DeliveryConfirmationService final : public platform::BusinessComponent {
public:
    DeliveryConfirmationService();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::fulfillment
