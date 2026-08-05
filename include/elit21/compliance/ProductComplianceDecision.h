#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::compliance {

class ProductComplianceDecision final : public platform::BusinessComponent {
public:
    ProductComplianceDecision();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::compliance
