#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::network {

class ResponseInterceptor final : public platform::BusinessComponent {
public:
    ResponseInterceptor();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::network
