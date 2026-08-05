#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::terminal {

class KeyboardHandler final : public platform::BusinessComponent {
public:
    KeyboardHandler();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::terminal
