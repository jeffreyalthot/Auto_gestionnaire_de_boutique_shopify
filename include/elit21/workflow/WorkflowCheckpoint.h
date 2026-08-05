#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::workflow {

class WorkflowCheckpoint final : public platform::BusinessComponent {
public:
    WorkflowCheckpoint();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::workflow
