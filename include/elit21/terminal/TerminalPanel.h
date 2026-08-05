#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::terminal {

class TerminalPanel final : public platform::BusinessComponent {
public:
    TerminalPanel();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::terminal
