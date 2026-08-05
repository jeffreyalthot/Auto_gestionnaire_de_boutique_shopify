#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::mapping {

class LanguageMapper final : public platform::BusinessComponent {
public:
    LanguageMapper();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::mapping
