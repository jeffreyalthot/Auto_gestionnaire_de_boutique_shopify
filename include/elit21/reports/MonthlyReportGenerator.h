#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::reports {

class MonthlyReportGenerator final : public platform::BusinessComponent {
public:
    MonthlyReportGenerator();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::reports
