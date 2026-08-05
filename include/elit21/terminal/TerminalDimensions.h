#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::terminal {

class TerminalDimensions final : public platform::BusinessComponent {
public:
    TerminalDimensions();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::terminal
