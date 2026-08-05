#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::orders {

class CustomerOrderLine final : public platform::BusinessComponent {
public:
    CustomerOrderLine();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::orders
