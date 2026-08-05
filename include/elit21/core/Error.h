#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::core {

class Error final : public platform::BusinessComponent {
public:
    Error();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::core
