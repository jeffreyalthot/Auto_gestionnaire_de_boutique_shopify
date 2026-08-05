#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::concurrency {

class JobHandle final : public platform::BusinessComponent {
public:
    JobHandle();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::concurrency
