#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::core {

class TimeUtils final : public platform::BusinessComponent {
public:
    TimeUtils();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::core
