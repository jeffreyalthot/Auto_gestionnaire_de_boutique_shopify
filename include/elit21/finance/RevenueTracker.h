#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::finance {

class RevenueTracker final : public platform::BusinessComponent {
public:
    RevenueTracker();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::finance
