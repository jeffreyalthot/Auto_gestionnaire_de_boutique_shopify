#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::orders {

class CustomerOrder final : public platform::BusinessComponent {
public:
    CustomerOrder();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::orders
