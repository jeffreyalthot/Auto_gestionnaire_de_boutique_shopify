#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::core {

class Timestamp final : public platform::BusinessComponent {
public:
    Timestamp();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::core
