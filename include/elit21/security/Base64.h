#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::security {

class Base64 final : public platform::BusinessComponent {
public:
    Base64();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::security
