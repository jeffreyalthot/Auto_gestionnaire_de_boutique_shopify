#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::customers {

class CustomerConsentService final : public platform::BusinessComponent {
public:
    CustomerConsentService();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::customers
