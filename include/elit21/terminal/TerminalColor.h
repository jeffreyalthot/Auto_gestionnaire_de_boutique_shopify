#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::terminal {

class TerminalColor final : public platform::BusinessComponent {
public:
    TerminalColor();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::terminal
