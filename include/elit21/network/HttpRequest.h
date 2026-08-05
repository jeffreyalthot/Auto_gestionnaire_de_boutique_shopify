#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::network {

class HttpRequest final : public platform::BusinessComponent {
public:
    HttpRequest();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::network
