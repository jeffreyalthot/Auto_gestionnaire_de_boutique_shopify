#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::network {

class HttpMethod final : public platform::BusinessComponent {
public:
    HttpMethod();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::network
