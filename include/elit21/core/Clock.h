#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::core {

class Clock final : public platform::BusinessComponent {
public:
    Clock();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::core
