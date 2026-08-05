#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::core {

class AtomicCounter final : public platform::BusinessComponent {
public:
    AtomicCounter();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::core
