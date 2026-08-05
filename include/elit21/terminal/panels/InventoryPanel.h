#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::terminal::panels {

class InventoryPanel final : public platform::BusinessComponent {
public:
    InventoryPanel();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::terminal::panels
