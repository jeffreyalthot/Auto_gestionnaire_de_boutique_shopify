#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::configuration {

class LoggingConfig final : public platform::BusinessComponent {
public:
    LoggingConfig();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::configuration
