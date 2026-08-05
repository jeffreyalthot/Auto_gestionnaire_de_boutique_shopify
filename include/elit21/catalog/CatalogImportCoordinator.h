#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::catalog {

class CatalogImportCoordinator final : public platform::BusinessComponent {
public:
    CatalogImportCoordinator();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::catalog
