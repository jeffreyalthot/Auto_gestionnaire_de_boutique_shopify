#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::fulfillment {

class TrackingEvent final : public platform::BusinessComponent {
public:
    TrackingEvent();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::fulfillment
