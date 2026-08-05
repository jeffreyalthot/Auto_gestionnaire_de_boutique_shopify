#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::resilience {

class RecoveryManager final : public platform::BusinessComponent {
public:
    RecoveryManager();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::resilience
