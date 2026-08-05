#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::network {

class HttpServer final : public platform::BusinessComponent {
public:
    HttpServer();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::network
