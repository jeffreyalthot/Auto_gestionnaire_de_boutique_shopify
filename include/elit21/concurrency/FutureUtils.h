#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::concurrency {

class FutureUtils final : public platform::BusinessComponent {
public:
    FutureUtils();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::concurrency
