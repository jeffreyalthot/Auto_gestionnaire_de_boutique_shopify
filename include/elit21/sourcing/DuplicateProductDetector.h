#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::sourcing {

class DuplicateProductDetector final : public platform::BusinessComponent {
public:
    DuplicateProductDetector();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::sourcing
