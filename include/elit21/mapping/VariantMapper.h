#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::mapping {

class VariantMapper final : public platform::BusinessComponent {
public:
    VariantMapper();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::mapping
