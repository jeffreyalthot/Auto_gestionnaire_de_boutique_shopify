#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::analytics {

class AnalyticsCoordinator final : public platform::BusinessComponent {
public:
    AnalyticsCoordinator();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::analytics
