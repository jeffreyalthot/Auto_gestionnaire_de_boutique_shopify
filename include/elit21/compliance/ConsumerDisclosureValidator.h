#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::compliance {

class ConsumerDisclosureValidator final : public platform::BusinessComponent {
public:
    ConsumerDisclosureValidator();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::compliance
