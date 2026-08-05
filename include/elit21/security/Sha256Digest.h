#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::security {

class Sha256Digest final : public platform::BusinessComponent {
public:
    Sha256Digest();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::security
