#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::scheduler {

class JobStatus final : public platform::BusinessComponent {
public:
    JobStatus();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::scheduler
