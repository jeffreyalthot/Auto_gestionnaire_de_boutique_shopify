#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::storage {

class CacheEntry final : public platform::BusinessComponent {
public:
    CacheEntry();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::storage
