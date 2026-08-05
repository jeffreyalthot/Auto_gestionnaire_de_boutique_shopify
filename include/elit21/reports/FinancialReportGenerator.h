#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::reports {

class FinancialReportGenerator final : public platform::BusinessComponent {
public:
    FinancialReportGenerator();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::reports
