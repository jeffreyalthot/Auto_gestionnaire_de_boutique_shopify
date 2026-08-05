#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::catalog {

class ProductCreationPlan final : public platform::BusinessComponent {
public:
    ProductCreationPlan();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::catalog
