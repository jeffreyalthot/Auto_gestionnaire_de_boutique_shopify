#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::terminal {

class TerminalRenderer final : public platform::BusinessComponent {
public:
    TerminalRenderer();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::terminal
