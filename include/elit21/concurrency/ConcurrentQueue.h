#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::concurrency {

class ConcurrentQueue final : public platform::BusinessComponent {
public:
    ConcurrentQueue();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::concurrency
