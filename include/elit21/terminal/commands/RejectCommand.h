#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::terminal::commands {

class RejectCommand final : public platform::BusinessComponent {
public:
    RejectCommand();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::terminal::commands
