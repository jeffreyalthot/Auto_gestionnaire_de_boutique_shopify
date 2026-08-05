#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::configuration {

class ApplicationConfig final : public platform::BusinessComponent {
public:
    ApplicationConfig();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::configuration
