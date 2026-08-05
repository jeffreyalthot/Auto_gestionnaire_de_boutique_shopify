#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::storage {

class Cache final : public platform::BusinessComponent {
public:
    Cache();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::storage
