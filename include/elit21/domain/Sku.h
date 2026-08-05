#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::domain {

class Sku final : public platform::BusinessComponent {
public:
    Sku();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::domain
