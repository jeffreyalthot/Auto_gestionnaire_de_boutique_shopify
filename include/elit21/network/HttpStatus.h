#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::network {

class HttpStatus final : public platform::BusinessComponent {
public:
    HttpStatus();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::network
