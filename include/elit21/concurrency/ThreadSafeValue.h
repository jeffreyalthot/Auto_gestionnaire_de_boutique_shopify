#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::concurrency {

class ThreadSafeValue final : public platform::BusinessComponent {
public:
    ThreadSafeValue();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::concurrency
