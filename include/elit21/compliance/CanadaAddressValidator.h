#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::compliance {

class CanadaAddressValidator final : public platform::BusinessComponent {
public:
    CanadaAddressValidator();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::compliance
