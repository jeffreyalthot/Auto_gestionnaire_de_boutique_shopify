#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::mapping {

class CollectionMapper final : public platform::BusinessComponent {
public:
    CollectionMapper();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::mapping
