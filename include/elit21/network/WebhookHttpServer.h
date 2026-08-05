#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::network {

class WebhookHttpServer final : public platform::BusinessComponent {
public:
    WebhookHttpServer();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::network
