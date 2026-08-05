#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::domain {

class TrackingNumber final : public platform::BusinessComponent {
public:
    TrackingNumber();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::domain
