#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::reports {

class InventoryReportGenerator final : public platform::BusinessComponent {
public:
    InventoryReportGenerator();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::reports
