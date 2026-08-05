#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::inventory {

class InventorySnapshot final : public platform::BusinessComponent {
public:
    InventorySnapshot();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::inventory
