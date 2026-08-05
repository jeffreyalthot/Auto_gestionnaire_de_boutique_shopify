#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::scheduler {

class JobContext final : public platform::BusinessComponent {
public:
    JobContext();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::scheduler
