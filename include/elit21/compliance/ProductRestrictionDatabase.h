#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::compliance {

class ProductRestrictionDatabase final : public platform::BusinessComponent {
public:
    ProductRestrictionDatabase();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::compliance
