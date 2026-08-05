#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::pricing {

class PriceHistoryService final : public platform::BusinessComponent {
public:
    PriceHistoryService();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::pricing
