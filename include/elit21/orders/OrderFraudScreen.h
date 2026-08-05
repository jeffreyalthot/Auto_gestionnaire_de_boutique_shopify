#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::orders {

class OrderFraudScreen final : public platform::BusinessComponent {
public:
    OrderFraudScreen();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::orders
