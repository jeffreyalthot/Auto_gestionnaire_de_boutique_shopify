#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::workflow {

class StateMachine final : public platform::BusinessComponent {
public:
    StateMachine();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::workflow
