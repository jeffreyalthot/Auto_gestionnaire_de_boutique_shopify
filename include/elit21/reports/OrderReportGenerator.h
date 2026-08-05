#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::reports {

class OrderReportGenerator final : public platform::BusinessComponent {
public:
    OrderReportGenerator();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::reports
