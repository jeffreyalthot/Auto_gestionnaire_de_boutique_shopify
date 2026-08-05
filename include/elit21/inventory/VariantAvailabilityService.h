#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::inventory {

class VariantAvailabilityService final : public platform::BusinessComponent {
public:
    VariantAvailabilityService();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::inventory
