#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::fulfillment {

class Shipment final : public platform::BusinessComponent {
public:
    Shipment();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::fulfillment
