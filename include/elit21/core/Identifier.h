#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::core {

class Identifier final : public platform::BusinessComponent {
public:
    Identifier();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::core
