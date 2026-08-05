#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::repositories {

class CustomerRepository final : public platform::BusinessComponent {
public:
    CustomerRepository();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::repositories
