#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::core {

class Optional final : public platform::BusinessComponent {
public:
    Optional();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::core
