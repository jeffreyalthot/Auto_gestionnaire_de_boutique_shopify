#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::returns {

class ReturnStateMachine final : public platform::BusinessComponent {
public:
    ReturnStateMachine();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::returns
