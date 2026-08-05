#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::terminal {

class AnsiTerminal final : public platform::BusinessComponent {
public:
    AnsiTerminal();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::terminal
