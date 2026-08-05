#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::analytics {

class PerformanceIndicator final : public platform::BusinessComponent {
public:
    PerformanceIndicator();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::analytics
