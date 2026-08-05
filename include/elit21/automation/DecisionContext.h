#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::automation {

class DecisionContext final : public platform::BusinessComponent {
public:
    DecisionContext();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::automation
