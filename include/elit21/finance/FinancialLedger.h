#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::finance {

class FinancialLedger final : public platform::BusinessComponent {
public:
    FinancialLedger();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::finance
