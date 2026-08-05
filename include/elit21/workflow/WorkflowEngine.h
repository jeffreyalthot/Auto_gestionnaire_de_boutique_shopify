#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::workflow {

class WorkflowEngine final : public platform::BusinessComponent {
public:
    WorkflowEngine();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::workflow
