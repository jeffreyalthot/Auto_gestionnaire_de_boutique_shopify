#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::orders {

class ShippingAddressSanitizer final : public platform::BusinessComponent {
public:
    ShippingAddressSanitizer();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::orders
