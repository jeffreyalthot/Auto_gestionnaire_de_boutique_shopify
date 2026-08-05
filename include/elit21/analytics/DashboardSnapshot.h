#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::analytics {

class DashboardSnapshot final : public platform::BusinessComponent {
public:
    DashboardSnapshot();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::analytics
