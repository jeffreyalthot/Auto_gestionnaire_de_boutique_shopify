#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::core {

class Status final : public platform::BusinessComponent {
public:
    Status();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::core
