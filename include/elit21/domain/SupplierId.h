#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::domain {

class SupplierId final : public platform::BusinessComponent {
public:
    SupplierId();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::domain
