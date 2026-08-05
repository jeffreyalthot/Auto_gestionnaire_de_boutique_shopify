#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::customers {

class CustomerRiskProfile final : public platform::BusinessComponent {
public:
    CustomerRiskProfile();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::customers
