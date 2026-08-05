#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::scheduler {

class SchedulerClock final : public platform::BusinessComponent {
public:
    SchedulerClock();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::scheduler
