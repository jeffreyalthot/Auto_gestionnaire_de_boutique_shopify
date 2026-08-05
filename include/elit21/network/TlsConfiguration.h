#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::network {

class TlsConfiguration final : public platform::BusinessComponent {
public:
    TlsConfiguration();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::network
