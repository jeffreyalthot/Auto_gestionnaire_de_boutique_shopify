#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::orders {

class PaymentAuthorizationGate final : public platform::BusinessComponent {
public:
    PaymentAuthorizationGate();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::orders
