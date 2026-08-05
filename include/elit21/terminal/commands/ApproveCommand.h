#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::terminal::commands {

class ApproveCommand final : public platform::BusinessComponent {
public:
    ApproveCommand();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::terminal::commands
