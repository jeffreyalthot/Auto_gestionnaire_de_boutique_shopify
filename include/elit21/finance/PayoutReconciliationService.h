#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::finance {

class PayoutReconciliationService final : public platform::BusinessComponent {
public:
    PayoutReconciliationService();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::finance
