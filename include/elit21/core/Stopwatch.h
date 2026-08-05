#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::core {

class Stopwatch final : public platform::BusinessComponent {
public:
    Stopwatch();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::core
