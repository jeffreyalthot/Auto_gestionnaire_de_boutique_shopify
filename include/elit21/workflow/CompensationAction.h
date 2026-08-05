#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::workflow {

class CompensationAction final : public platform::BusinessComponent {
public:
    CompensationAction();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::workflow
