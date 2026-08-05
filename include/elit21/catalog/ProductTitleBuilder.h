#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::catalog {

class ProductTitleBuilder final : public platform::BusinessComponent {
public:
    ProductTitleBuilder();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::catalog
