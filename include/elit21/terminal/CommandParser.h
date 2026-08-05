#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::terminal {

class CommandParser final : public platform::BusinessComponent {
public:
    CommandParser();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::terminal
