#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::workflow {

class WorkflowInstance final : public platform::BusinessComponent {
public:
    WorkflowInstance();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::workflow
