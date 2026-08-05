#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::domain {

class Currency final : public platform::BusinessComponent {
public:
    Currency();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::domain
