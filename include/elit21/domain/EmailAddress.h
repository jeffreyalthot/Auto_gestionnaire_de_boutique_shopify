#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::domain {

class EmailAddress final : public platform::BusinessComponent {
public:
    EmailAddress();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::domain
