#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::network {

class HttpResponse final : public platform::BusinessComponent {
public:
    HttpResponse();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::network
