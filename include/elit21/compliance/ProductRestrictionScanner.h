#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::compliance {

class ProductRestrictionScanner final : public platform::BusinessComponent {
public:
    ProductRestrictionScanner();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::compliance
