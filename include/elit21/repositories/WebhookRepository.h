#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::repositories {

class WebhookRepository final : public platform::BusinessComponent {
public:
    WebhookRepository();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::repositories
