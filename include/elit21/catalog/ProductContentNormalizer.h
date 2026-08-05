#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::catalog {

class ProductContentNormalizer final : public platform::BusinessComponent {
public:
    ProductContentNormalizer();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::catalog
