#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::inventory {

class StockPollingPolicy final : public platform::BusinessComponent {
public:
    StockPollingPolicy();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::inventory
