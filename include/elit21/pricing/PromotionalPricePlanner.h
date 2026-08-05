#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::pricing {

class PromotionalPricePlanner final : public platform::BusinessComponent {
public:
    PromotionalPricePlanner();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::pricing
