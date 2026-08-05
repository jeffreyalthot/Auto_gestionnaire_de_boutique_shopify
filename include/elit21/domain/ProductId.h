#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::domain {

class ProductId final : public platform::BusinessComponent {
public:
    ProductId();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::domain
