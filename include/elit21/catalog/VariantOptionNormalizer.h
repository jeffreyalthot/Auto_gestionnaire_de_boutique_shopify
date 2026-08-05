#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::catalog {

class VariantOptionNormalizer final : public platform::BusinessComponent {
public:
    VariantOptionNormalizer();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::catalog
