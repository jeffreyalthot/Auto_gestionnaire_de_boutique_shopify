#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::security {

class Md5Digest final : public platform::BusinessComponent {
public:
    Md5Digest();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::security
