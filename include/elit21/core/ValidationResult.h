#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::core {

class ValidationResult final : public platform::BusinessComponent {
public:
    ValidationResult();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::core
