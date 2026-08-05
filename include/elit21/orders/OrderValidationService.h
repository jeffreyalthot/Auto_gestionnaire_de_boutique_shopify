#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::orders {

class OrderValidationService final : public platform::BusinessComponent {
public:
    OrderValidationService();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::orders
