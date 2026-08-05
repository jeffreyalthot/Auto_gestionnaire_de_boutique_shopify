#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::concurrency {

class WorkerThread final : public platform::BusinessComponent {
public:
    WorkerThread();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::concurrency
