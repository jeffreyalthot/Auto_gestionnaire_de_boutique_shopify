#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::repositories {

class WorkflowInstanceRepository final : public platform::BusinessComponent {
public:
    WorkflowInstanceRepository();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::repositories
