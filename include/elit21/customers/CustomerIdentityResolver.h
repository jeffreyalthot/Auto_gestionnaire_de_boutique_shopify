#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::customers {

class CustomerIdentityResolver final : public platform::BusinessComponent {
public:
    CustomerIdentityResolver();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::customers
