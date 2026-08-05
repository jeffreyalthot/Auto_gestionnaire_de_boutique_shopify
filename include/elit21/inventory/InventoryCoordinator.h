#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::inventory {

class InventoryCoordinator final : public platform::BusinessComponent {
public:
    InventoryCoordinator();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::inventory
