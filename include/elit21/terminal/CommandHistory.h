#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::terminal {

class CommandHistory final : public platform::BusinessComponent {
public:
    CommandHistory();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::terminal
