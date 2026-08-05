#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::resilience {

class FailureClassifier final : public platform::BusinessComponent {
public:
    FailureClassifier();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::resilience
