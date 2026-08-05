#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::mapping {

class CurrencyMapper final : public platform::BusinessComponent {
public:
    CurrencyMapper();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::mapping
