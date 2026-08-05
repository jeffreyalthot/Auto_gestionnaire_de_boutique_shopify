#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::mapping {

class ShippingMethodMapper final : public platform::BusinessComponent {
public:
    ShippingMethodMapper();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::mapping
