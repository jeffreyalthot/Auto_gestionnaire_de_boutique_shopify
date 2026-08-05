#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::network {

class HttpClientPool final : public platform::BusinessComponent {
public:
    HttpClientPool();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::network
