#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::pricing {

class PriceChangeDecision final : public platform::BusinessComponent {
public:
    PriceChangeDecision();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::pricing
