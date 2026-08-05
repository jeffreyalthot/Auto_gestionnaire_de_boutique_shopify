#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::configuration {

class SecurityConfig final : public platform::BusinessComponent {
public:
    SecurityConfig();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::configuration
