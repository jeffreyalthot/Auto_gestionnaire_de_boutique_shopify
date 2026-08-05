#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::automation {

class AutonomousPolicy final : public platform::BusinessComponent {
public:
    AutonomousPolicy();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::automation
