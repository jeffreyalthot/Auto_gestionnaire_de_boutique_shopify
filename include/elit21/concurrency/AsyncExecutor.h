#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::concurrency {

class AsyncExecutor final : public platform::BusinessComponent {
public:
    AsyncExecutor();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::concurrency
