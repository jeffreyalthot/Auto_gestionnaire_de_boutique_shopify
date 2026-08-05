#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::workflow {

class WorkflowRepository final : public platform::BusinessComponent {
public:
    WorkflowRepository();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::workflow
