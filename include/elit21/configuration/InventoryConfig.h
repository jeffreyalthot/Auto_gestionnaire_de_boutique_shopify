#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::configuration {

class InventoryConfig final : public platform::BusinessComponent {
public:
    InventoryConfig();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::configuration
