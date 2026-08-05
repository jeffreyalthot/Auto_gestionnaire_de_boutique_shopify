#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::catalog {

class ProductTranslationService final : public platform::BusinessComponent {
public:
    ProductTranslationService();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::catalog
