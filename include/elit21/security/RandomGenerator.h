#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::security {

class RandomGenerator final : public platform::BusinessComponent {
public:
    RandomGenerator();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::security
