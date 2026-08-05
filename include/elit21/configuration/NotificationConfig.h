#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::configuration {

class NotificationConfig final : public platform::BusinessComponent {
public:
    NotificationConfig();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::configuration
