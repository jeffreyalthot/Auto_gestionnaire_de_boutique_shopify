#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::storage {

class Repository final : public platform::BusinessComponent {
public:
    Repository();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::storage
