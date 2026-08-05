#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::workflow {

class WorkflowState final : public platform::BusinessComponent {
public:
    WorkflowState();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::workflow
