#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::workflow {

class Workflow final : public platform::BusinessComponent {
public:
    Workflow();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::workflow
