#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::mapping {

class OrderStatusMapper final : public platform::BusinessComponent {
public:
    OrderStatusMapper();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::mapping
