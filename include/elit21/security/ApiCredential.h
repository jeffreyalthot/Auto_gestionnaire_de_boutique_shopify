#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::security {

class ApiCredential final : public platform::BusinessComponent {
public:
    ApiCredential();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::security
