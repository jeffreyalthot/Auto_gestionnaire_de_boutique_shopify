#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::catalog {

class CollectionAssignmentEngine final : public platform::BusinessComponent {
public:
    CollectionAssignmentEngine();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::catalog
