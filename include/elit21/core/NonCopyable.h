#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::core {

class NonCopyable final : public platform::BusinessComponent {
public:
    NonCopyable();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::core
