#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::repositories {

class InventoryRepository final : public platform::BusinessComponent {
public:
    InventoryRepository();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::repositories
