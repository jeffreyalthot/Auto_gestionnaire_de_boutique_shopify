#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::inventory {

class InventoryStalenessDetector final : public platform::BusinessComponent {
public:
    InventoryStalenessDetector();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::inventory
