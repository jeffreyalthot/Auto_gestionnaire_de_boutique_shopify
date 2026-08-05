#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::finance {

class FinancialReconciliationService final : public platform::BusinessComponent {
public:
    FinancialReconciliationService();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::finance
