#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::security {

class SignatureVerifier final : public platform::BusinessComponent {
public:
    SignatureVerifier();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::security
