#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::domain {

class PostalCode final : public platform::BusinessComponent {
public:
    PostalCode();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::domain
