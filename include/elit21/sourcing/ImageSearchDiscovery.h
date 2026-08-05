#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::sourcing {

class ImageSearchDiscovery final : public platform::BusinessComponent {
public:
    ImageSearchDiscovery();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::sourcing
