#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::inventory {

class ProductAvailabilityService final : public platform::BusinessComponent {
public:
    ProductAvailabilityService();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::inventory
