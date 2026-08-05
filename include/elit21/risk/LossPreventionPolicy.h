#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::risk {

class LossPreventionPolicy final : public platform::BusinessComponent {
public:
    LossPreventionPolicy();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::risk
