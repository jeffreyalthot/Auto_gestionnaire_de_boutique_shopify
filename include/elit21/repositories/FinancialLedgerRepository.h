#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::repositories {

class FinancialLedgerRepository final : public platform::BusinessComponent {
public:
    FinancialLedgerRepository();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::repositories
