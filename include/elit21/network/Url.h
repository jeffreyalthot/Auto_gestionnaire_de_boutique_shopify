#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::network {

class Url final : public platform::BusinessComponent {
public:
    Url();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::network
