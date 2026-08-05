#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::mapping {

class AddressMapper final : public platform::BusinessComponent {
public:
    AddressMapper();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::mapping
