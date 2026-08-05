#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::mapping {

class SkuMapper final : public platform::BusinessComponent {
public:
    SkuMapper();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::mapping
