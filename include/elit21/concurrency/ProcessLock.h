#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::concurrency {

class ProcessLock final : public platform::BusinessComponent {
public:
    ProcessLock();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::concurrency
