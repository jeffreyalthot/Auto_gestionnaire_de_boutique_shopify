#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::core {

class DateTime final : public platform::BusinessComponent {
public:
    DateTime();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::core
