#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::mapping {

class CarrierMapper final : public platform::BusinessComponent {
public:
    CarrierMapper();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::mapping
