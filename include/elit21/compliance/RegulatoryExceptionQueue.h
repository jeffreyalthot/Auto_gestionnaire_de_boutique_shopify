#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::compliance {

class RegulatoryExceptionQueue final : public platform::BusinessComponent {
public:
    RegulatoryExceptionQueue();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::compliance
