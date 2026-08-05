#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::orders {

class SupplierOrder final : public platform::BusinessComponent {
public:
    SupplierOrder();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::orders
