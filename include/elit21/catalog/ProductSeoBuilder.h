#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::catalog {

class ProductSeoBuilder final : public platform::BusinessComponent {
public:
    ProductSeoBuilder();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::catalog
