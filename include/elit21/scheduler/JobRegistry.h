#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::scheduler {

class JobRegistry final : public platform::BusinessComponent {
public:
    JobRegistry();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::scheduler
