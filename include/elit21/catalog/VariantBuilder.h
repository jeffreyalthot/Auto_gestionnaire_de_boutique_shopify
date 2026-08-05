#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::catalog {

class VariantBuilder final : public platform::BusinessComponent {
public:
    VariantBuilder();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::catalog
