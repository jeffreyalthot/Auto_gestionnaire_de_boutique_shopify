#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::mapping {

class CategoryMapper final : public platform::BusinessComponent {
public:
    CategoryMapper();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::mapping
