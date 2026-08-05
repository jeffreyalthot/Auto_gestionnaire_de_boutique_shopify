#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::repositories {

class ShipmentRepository final : public platform::BusinessComponent {
public:
    ShipmentRepository();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::repositories
