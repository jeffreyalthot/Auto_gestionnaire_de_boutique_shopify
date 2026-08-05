#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::terminal {

class ConsoleOutput final : public platform::BusinessComponent {
public:
    ConsoleOutput();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::terminal
