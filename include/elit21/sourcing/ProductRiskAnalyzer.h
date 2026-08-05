#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::sourcing {

class ProductRiskAnalyzer final : public platform::BusinessComponent {
public:
    ProductRiskAnalyzer();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::sourcing
