#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::scheduler {

class ScheduledJob final : public platform::BusinessComponent {
public:
    ScheduledJob();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::scheduler
