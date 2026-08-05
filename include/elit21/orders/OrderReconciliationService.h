#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::orders {

class OrderReconciliationService final : public platform::BusinessComponent {
public:
    OrderReconciliationService();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::orders
