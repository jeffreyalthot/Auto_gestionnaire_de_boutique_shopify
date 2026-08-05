#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::security {

class ConstantTimeCompare final : public platform::BusinessComponent {
public:
    ConstantTimeCompare();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::security
