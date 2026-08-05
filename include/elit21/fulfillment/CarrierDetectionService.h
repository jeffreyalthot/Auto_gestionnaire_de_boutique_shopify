#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::fulfillment {

class CarrierDetectionService final : public platform::BusinessComponent {
public:
    CarrierDetectionService();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::fulfillment
