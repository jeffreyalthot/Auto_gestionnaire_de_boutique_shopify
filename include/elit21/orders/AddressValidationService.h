#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::orders {

class AddressValidationService final : public platform::BusinessComponent {
public:
    AddressValidationService();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::orders
