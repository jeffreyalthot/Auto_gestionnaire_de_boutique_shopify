#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::customers {

class Customer final : public platform::BusinessComponent {
public:
    Customer();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::customers
