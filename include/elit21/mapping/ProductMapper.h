#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::mapping {

class ProductMapper final : public platform::BusinessComponent {
public:
    ProductMapper();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::mapping
