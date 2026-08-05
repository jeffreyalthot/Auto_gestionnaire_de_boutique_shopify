#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::inventory {

class AvailabilityDecision final : public platform::BusinessComponent {
public:
    AvailabilityDecision();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::inventory
