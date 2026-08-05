#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::pricing {

class PriceReconciliationService final : public platform::BusinessComponent {
public:
    PriceReconciliationService();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::pricing
