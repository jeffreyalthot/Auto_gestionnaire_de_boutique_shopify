#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::scheduler {

class RecurringJob final : public platform::BusinessComponent {
public:
    RecurringJob();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::scheduler
