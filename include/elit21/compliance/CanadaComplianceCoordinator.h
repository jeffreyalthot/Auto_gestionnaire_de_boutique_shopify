#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::compliance {

class CanadaComplianceCoordinator final : public platform::BusinessComponent {
public:
    CanadaComplianceCoordinator();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::compliance
