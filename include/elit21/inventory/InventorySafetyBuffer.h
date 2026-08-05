#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::inventory {

class InventorySafetyBuffer final : public platform::BusinessComponent {
public:
    InventorySafetyBuffer();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::inventory
