#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::reports {

class DailyReportGenerator final : public platform::BusinessComponent {
public:
    DailyReportGenerator();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::reports
