#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::observability {

class AlertDispatcher final : public platform::BusinessComponent {
public:
    AlertDispatcher();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::observability
