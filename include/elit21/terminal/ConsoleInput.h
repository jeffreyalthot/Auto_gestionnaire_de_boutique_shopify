#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::terminal {

class ConsoleInput final : public platform::BusinessComponent {
public:
    ConsoleInput();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::terminal
