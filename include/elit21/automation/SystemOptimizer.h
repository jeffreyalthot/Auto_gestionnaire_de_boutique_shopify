#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::automation {

class SystemOptimizer final : public platform::BusinessComponent {
public:
    SystemOptimizer();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::automation
