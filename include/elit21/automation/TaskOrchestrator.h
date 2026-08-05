#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::automation {

class TaskOrchestrator final : public platform::BusinessComponent {
public:
    TaskOrchestrator();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::automation
