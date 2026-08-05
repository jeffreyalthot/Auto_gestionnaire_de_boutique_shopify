#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::configuration {

class ConfigurationLoader final : public platform::BusinessComponent {
public:
    ConfigurationLoader();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::configuration
