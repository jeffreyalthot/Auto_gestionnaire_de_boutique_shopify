#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::network {

class MultipartBuilder final : public platform::BusinessComponent {
public:
    MultipartBuilder();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::network
