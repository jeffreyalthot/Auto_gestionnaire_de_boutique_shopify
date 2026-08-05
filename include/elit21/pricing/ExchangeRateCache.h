#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::pricing {

class ExchangeRateCache final : public platform::BusinessComponent {
public:
    ExchangeRateCache();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::pricing
