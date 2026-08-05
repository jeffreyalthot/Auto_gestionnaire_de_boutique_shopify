#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::finance {

class AccountingEntry final : public platform::BusinessComponent {
public:
    AccountingEntry();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::finance
