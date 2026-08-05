#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::workflow {

class StateTransition final : public platform::BusinessComponent {
public:
    StateTransition();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::workflow
