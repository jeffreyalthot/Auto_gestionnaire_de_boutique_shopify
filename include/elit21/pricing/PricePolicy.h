#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::pricing {

class PricePolicy final : public platform::BusinessComponent {
public:
    PricePolicy();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::pricing
