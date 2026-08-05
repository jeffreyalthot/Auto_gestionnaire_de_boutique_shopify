#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::inventory {

class InventoryReconciliationService final : public platform::BusinessComponent {
public:
    InventoryReconciliationService();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::inventory
