#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::workflow {

class ActivityContext final : public platform::BusinessComponent {
public:
    ActivityContext();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::workflow
