#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::pricing {

class PriceRoundingPolicy final : public platform::BusinessComponent {
public:
    PriceRoundingPolicy();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::pricing
