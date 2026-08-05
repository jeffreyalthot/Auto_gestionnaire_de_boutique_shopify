#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::mapping {

class VariantMapping final : public platform::BusinessComponent {
public:
    VariantMapping();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::mapping
