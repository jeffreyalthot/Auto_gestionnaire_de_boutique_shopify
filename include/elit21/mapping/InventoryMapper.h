#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::mapping {

class InventoryMapper final : public platform::BusinessComponent {
public:
    InventoryMapper();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::mapping
