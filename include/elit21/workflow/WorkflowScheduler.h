#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::workflow {

class WorkflowScheduler final : public platform::BusinessComponent {
public:
    WorkflowScheduler();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::workflow
