#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::repositories {

class ProductRepository final : public platform::BusinessComponent {
public:
    ProductRepository();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::repositories
