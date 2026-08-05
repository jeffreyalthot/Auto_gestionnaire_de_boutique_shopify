#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::concurrency {

class CancellationSource final : public platform::BusinessComponent {
public:
    CancellationSource();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::concurrency
