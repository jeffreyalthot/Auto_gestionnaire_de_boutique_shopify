#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::messaging {

class CommandBus final : public platform::BusinessComponent {
public:
    CommandBus();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::messaging
