#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::sourcing {

class SourcingPolicy final : public platform::BusinessComponent {
public:
    SourcingPolicy();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::sourcing
