#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::analytics {

class MarginAnalytics final : public platform::BusinessComponent {
public:
    MarginAnalytics();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::analytics
