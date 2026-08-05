#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::mapping {

class CustomerMapper final : public platform::BusinessComponent {
public:
    CustomerMapper();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::mapping
