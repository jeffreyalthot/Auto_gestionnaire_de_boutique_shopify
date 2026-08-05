#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::network {

class NetworkError final : public platform::BusinessComponent {
public:
    NetworkError();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::network
