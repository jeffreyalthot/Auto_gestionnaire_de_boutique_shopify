#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::workflow {

class SagaCoordinator final : public platform::BusinessComponent {
public:
    SagaCoordinator();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::workflow
