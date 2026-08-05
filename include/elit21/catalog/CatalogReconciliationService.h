#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::catalog {

class CatalogReconciliationService final : public platform::BusinessComponent {
public:
    CatalogReconciliationService();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::catalog
