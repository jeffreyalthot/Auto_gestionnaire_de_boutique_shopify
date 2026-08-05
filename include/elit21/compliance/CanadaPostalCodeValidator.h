#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::compliance {

class CanadaPostalCodeValidator final : public platform::BusinessComponent {
public:
    CanadaPostalCodeValidator();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::compliance
