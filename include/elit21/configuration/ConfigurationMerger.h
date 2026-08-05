#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::configuration {

class ConfigurationMerger final : public platform::BusinessComponent {
public:
    ConfigurationMerger();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::configuration
