#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::reports {

class WeeklyReportGenerator final : public platform::BusinessComponent {
public:
    WeeklyReportGenerator();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::reports
