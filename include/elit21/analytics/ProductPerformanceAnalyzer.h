#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::analytics {

class ProductPerformanceAnalyzer final : public platform::BusinessComponent {
public:
    ProductPerformanceAnalyzer();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::analytics
