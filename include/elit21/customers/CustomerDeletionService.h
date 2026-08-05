#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::customers {

class CustomerDeletionService final : public platform::BusinessComponent {
public:
    CustomerDeletionService();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::customers
