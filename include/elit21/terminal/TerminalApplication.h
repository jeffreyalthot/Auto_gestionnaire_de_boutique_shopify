#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::terminal {

class TerminalApplication final : public platform::BusinessComponent {
public:
    TerminalApplication();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::terminal
