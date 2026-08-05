#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::customer_service {

class EmailNotificationClient final : public platform::BusinessComponent {
public:
    EmailNotificationClient();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::customer_service
