#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::orders {

class OrderPlacementCoordinator final : public platform::BusinessComponent {
public:
    OrderPlacementCoordinator();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::orders
