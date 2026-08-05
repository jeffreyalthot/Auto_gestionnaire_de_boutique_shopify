#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::storage {

class Entity final : public platform::BusinessComponent {
public:
    Entity();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::storage
