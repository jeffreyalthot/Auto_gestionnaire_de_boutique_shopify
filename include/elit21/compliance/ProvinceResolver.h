#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::compliance {

class ProvinceResolver final : public platform::BusinessComponent {
public:
    ProvinceResolver();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::compliance
