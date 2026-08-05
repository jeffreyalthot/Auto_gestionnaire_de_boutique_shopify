#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::analytics {

class RevenueAnalytics final : public platform::BusinessComponent {
public:
    RevenueAnalytics();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::analytics
