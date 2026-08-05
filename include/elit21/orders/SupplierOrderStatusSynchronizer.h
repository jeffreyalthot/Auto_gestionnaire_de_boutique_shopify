#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::orders {

class SupplierOrderStatusSynchronizer final : public platform::BusinessComponent {
public:
    SupplierOrderStatusSynchronizer();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::orders
