#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::analytics {

class SupplierPerformanceAnalyzer final : public platform::BusinessComponent {
public:
    SupplierPerformanceAnalyzer();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::analytics
