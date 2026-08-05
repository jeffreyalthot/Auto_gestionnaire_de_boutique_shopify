#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::terminal {

class TerminalCommand final : public platform::BusinessComponent {
public:
    TerminalCommand();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::terminal
