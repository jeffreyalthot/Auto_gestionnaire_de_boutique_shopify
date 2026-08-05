#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::customers {

class CustomerAnonymizationService final : public platform::BusinessComponent {
public:
    CustomerAnonymizationService();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::customers
