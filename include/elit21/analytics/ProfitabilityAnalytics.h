#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::analytics {

class ProfitabilityAnalytics final : public platform::BusinessComponent {
public:
    ProfitabilityAnalytics();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::analytics
