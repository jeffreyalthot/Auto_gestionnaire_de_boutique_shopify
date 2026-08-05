#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::storage {

class Row final : public platform::BusinessComponent {
public:
    Row();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::storage
