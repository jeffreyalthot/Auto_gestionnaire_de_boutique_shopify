#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::inventory {

class InventoryLevel final : public platform::BusinessComponent {
public:
    InventoryLevel();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::inventory
