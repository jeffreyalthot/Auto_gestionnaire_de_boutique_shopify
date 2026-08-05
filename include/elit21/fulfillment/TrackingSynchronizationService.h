#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::fulfillment {

class TrackingSynchronizationService final : public platform::BusinessComponent {
public:
    TrackingSynchronizationService();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::fulfillment
