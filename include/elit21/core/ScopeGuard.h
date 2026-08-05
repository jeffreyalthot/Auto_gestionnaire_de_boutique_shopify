#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::core {

class ScopeGuard final : public platform::BusinessComponent {
public:
    ScopeGuard();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::core
