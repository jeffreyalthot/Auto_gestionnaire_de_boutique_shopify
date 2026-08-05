#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::sourcing {

class ProductQualityAnalyzer final : public platform::BusinessComponent {
public:
    ProductQualityAnalyzer();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::sourcing
