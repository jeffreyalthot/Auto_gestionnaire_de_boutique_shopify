#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::orders {

class OrderMappingService final : public platform::BusinessComponent {
public:
    OrderMappingService();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::orders
