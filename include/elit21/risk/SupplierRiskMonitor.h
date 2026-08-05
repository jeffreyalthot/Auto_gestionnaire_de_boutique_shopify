#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::risk {

class SupplierRiskMonitor final : public platform::BusinessComponent {
public:
    SupplierRiskMonitor();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::risk
