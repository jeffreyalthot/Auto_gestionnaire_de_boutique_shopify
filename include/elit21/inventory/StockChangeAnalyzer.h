#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::inventory {

class StockChangeAnalyzer final : public platform::BusinessComponent {
public:
    StockChangeAnalyzer();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::inventory
