#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::network {

class JsonHttpClient final : public platform::BusinessComponent {
public:
    JsonHttpClient();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::network
