#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::workflow {

class ActivityResult final : public platform::BusinessComponent {
public:
    ActivityResult();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::workflow
