#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::catalog {

class ProductImportService final : public platform::BusinessComponent {
public:
    ProductImportService();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::catalog
