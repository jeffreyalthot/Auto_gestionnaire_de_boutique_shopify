#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::pricing {

class MinimumPriceGuard final : public platform::BusinessComponent {
public:
    MinimumPriceGuard();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::pricing
