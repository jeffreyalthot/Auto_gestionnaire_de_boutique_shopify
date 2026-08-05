#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::terminal {

class TerminalEvent final : public platform::BusinessComponent {
public:
    TerminalEvent();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::terminal
