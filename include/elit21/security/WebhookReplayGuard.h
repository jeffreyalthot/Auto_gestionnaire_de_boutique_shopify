#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::security {

class WebhookReplayGuard final : public platform::BusinessComponent {
public:
    WebhookReplayGuard();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::security
