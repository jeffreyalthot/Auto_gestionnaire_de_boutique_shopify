#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::network {

class Router final : public platform::BusinessComponent {
public:
    Router();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::network
