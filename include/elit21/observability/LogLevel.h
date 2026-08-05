#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::observability {

class LogLevel final : public platform::BusinessComponent {
public:
    LogLevel();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::observability
