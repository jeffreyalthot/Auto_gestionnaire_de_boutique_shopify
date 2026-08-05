#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::core {

class Expected final : public platform::BusinessComponent {
public:
    Expected();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::core
