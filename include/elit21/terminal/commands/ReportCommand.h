#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::terminal::commands {

class ReportCommand final : public platform::BusinessComponent {
public:
    ReportCommand();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::terminal::commands
