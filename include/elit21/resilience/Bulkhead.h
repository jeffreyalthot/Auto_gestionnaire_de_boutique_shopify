#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::resilience {

class Bulkhead final : public platform::BusinessComponent {
public:
    Bulkhead();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::resilience
