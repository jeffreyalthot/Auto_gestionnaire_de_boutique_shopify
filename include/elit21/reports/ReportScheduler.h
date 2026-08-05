#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::reports {

class ReportScheduler final : public platform::BusinessComponent {
public:
    ReportScheduler();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::reports
