#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::finance {

class ExpenseTracker final : public platform::BusinessComponent {
public:
    ExpenseTracker();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::finance
