#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::pricing {

class CurrencyConversionService final : public platform::BusinessComponent {
public:
    CurrencyConversionService();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::pricing
