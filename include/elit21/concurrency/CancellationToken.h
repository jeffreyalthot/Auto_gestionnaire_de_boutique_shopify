#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::concurrency {

class CancellationToken final : public platform::BusinessComponent {
public:
    CancellationToken();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::concurrency
