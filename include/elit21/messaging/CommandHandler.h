#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::messaging {

class CommandHandler final : public platform::BusinessComponent {
public:
    CommandHandler();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::messaging
