#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::network {

class HttpClient final : public platform::BusinessComponent {
public:
    HttpClient();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::network
