#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::network {

class HeaderCollection final : public platform::BusinessComponent {
public:
    HeaderCollection();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::network
