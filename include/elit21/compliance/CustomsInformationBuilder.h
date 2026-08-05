#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::compliance {

class CustomsInformationBuilder final : public platform::BusinessComponent {
public:
    CustomsInformationBuilder();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::compliance
