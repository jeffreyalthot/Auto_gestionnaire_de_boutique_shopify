#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::security {

class AccessToken final : public platform::BusinessComponent {
public:
    AccessToken();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::security
