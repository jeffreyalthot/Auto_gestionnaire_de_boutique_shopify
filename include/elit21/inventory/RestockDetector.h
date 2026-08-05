#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::inventory {

class RestockDetector final : public platform::BusinessComponent {
public:
    RestockDetector();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::inventory
