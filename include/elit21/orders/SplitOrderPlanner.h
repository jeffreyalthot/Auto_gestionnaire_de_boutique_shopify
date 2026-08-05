#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::orders {

class SplitOrderPlanner final : public platform::BusinessComponent {
public:
    SplitOrderPlanner();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::orders
