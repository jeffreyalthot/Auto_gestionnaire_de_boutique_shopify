#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::reports {

class SupplierReportGenerator final : public platform::BusinessComponent {
public:
    SupplierReportGenerator();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::reports
