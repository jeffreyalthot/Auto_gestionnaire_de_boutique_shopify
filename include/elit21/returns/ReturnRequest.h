#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::returns {

class ReturnRequest final : public platform::BusinessComponent {
public:
    ReturnRequest();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::returns
