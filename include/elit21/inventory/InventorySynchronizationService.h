#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::inventory {

class InventorySynchronizationService final : public platform::BusinessComponent {
public:
    InventorySynchronizationService();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::inventory
