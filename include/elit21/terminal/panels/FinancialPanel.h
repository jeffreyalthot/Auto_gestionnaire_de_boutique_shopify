#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::terminal::panels {

class FinancialPanel final : public platform::BusinessComponent {
public:
    FinancialPanel();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::terminal::panels
