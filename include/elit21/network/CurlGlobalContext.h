#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::network {

class CurlGlobalContext final : public platform::BusinessComponent {
public:
    CurlGlobalContext();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::network
