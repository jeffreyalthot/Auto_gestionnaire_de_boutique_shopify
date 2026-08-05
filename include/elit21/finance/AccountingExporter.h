#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::finance {

class AccountingExporter final : public platform::BusinessComponent {
public:
    AccountingExporter();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::finance
