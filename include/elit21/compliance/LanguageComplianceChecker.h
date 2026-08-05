#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::compliance {

class LanguageComplianceChecker final : public platform::BusinessComponent {
public:
    LanguageComplianceChecker();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::compliance
