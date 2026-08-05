#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::domain {

class Money final : public platform::BusinessComponent {
public:
    Money();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::domain
