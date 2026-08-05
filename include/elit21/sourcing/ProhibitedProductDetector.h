#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::sourcing {

class ProhibitedProductDetector final : public platform::BusinessComponent {
public:
    ProhibitedProductDetector();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::sourcing
