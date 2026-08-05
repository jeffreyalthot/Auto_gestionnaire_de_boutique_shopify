#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::catalog {

class MetafieldBuilder final : public platform::BusinessComponent {
public:
    MetafieldBuilder();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::catalog
