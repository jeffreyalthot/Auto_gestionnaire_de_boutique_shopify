#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::fulfillment {

class TrackingPollingPolicy final : public platform::BusinessComponent {
public:
    TrackingPollingPolicy();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::fulfillment
