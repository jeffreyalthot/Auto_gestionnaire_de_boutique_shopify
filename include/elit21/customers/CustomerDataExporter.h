#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::customers {

class CustomerDataExporter final : public platform::BusinessComponent {
public:
    CustomerDataExporter();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::customers
