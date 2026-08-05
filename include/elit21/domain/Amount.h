#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::domain {

class Amount final : public platform::BusinessComponent {
public:
    Amount();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::domain
