#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::network {

class FormEncoder final : public platform::BusinessComponent {
public:
    FormEncoder();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::network
