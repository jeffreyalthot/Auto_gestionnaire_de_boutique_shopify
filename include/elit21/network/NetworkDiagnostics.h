#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::network {

class NetworkDiagnostics final : public platform::BusinessComponent {
public:
    NetworkDiagnostics();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::network
