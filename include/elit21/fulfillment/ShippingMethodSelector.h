#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::fulfillment {

class ShippingMethodSelector final : public platform::BusinessComponent {
public:
    ShippingMethodSelector();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::fulfillment
