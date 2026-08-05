#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::core {

class StringUtils final : public platform::BusinessComponent {
public:
    StringUtils();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::core
