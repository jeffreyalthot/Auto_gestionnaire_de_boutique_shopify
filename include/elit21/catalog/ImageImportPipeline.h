#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::catalog {

class ImageImportPipeline final : public platform::BusinessComponent {
public:
    ImageImportPipeline();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::catalog
