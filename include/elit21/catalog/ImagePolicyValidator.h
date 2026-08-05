#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::catalog {

class ImagePolicyValidator final : public platform::BusinessComponent {
public:
    ImagePolicyValidator();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::catalog
