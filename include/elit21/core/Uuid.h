#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::core {

class Uuid final : public platform::BusinessComponent {
public:
    Uuid();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::core
