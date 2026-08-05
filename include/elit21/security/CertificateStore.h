#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::security {

class CertificateStore final : public platform::BusinessComponent {
public:
    CertificateStore();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::security
