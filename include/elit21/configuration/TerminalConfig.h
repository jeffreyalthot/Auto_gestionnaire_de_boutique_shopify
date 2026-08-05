#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::configuration {

class TerminalConfig final : public platform::BusinessComponent {
public:
    TerminalConfig();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::configuration
