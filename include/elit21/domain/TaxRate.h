#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::domain {

class TaxRate final : public platform::BusinessComponent {
public:
    TaxRate();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::domain
