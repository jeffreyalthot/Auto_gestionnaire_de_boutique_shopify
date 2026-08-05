#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::terminal::panels {

class LogPanel final : public platform::BusinessComponent {
public:
    LogPanel();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::terminal::panels
