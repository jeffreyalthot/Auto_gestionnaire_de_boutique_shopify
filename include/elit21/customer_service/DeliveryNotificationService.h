#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::customer_service {

class DeliveryNotificationService final : public platform::BusinessComponent {
public:
    DeliveryNotificationService();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::customer_service
