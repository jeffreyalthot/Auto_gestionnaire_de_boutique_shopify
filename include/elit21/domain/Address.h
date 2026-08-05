#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::domain {

class Address final : public platform::BusinessComponent {
public:
    Address();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::domain
