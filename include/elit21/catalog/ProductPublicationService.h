#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::catalog {

class ProductPublicationService final : public platform::BusinessComponent {
public:
    ProductPublicationService();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::catalog
