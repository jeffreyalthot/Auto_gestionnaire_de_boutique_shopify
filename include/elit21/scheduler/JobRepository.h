#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::scheduler {

class JobRepository final : public platform::BusinessComponent {
public:
    JobRepository();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::scheduler
