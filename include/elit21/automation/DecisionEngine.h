#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::automation {

class DecisionEngine final : public platform::BusinessComponent {
public:
    DecisionEngine();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::automation
