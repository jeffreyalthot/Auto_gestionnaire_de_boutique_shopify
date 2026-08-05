#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::domain {

class LanguageCode final : public platform::BusinessComponent {
public:
    LanguageCode();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::domain
