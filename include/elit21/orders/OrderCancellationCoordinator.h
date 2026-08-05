#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::orders {

class OrderCancellationCoordinator final : public platform::BusinessComponent {
public:
    OrderCancellationCoordinator();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::orders
