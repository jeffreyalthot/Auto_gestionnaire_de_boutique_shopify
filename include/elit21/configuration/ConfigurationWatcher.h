#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::configuration {

class ConfigurationWatcher final : public platform::BusinessComponent {
public:
    ConfigurationWatcher();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::configuration
