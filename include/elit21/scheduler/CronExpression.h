#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::scheduler {

class CronExpression final : public platform::BusinessComponent {
public:
    CronExpression();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::scheduler
