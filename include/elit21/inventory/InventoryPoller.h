#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::inventory {

class InventoryPoller final : public platform::BusinessComponent {
public:
    InventoryPoller();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::inventory
