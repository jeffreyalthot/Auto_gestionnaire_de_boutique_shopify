#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::terminal {

class StatusCounter final : public platform::BusinessComponent {
public:
    StatusCounter();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::terminal
