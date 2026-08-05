#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::catalog {

class DescriptionGenerator final : public platform::BusinessComponent {
public:
    DescriptionGenerator();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::catalog
