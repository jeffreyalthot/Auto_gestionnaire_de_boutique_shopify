#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::configuration {

class ConfigurationValidator final : public platform::BusinessComponent {
public:
    ConfigurationValidator();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::configuration
