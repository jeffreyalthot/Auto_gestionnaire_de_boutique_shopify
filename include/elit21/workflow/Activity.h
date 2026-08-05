#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::workflow {

class Activity final : public platform::BusinessComponent {
public:
    Activity();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::workflow
