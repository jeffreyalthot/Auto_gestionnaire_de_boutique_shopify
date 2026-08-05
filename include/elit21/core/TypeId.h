#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::core {

class TypeId final : public platform::BusinessComponent {
public:
    TypeId();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::core
