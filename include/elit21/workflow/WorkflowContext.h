#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::workflow {

class WorkflowContext final : public platform::BusinessComponent {
public:
    WorkflowContext();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::workflow
