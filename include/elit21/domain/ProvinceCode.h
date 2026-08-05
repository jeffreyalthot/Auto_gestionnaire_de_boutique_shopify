#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::domain {

class ProvinceCode final : public platform::BusinessComponent {
public:
    ProvinceCode();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::domain
