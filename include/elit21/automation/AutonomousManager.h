#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::automation {

class AutonomousManager final : public platform::BusinessComponent {
public:
    AutonomousManager();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::automation
