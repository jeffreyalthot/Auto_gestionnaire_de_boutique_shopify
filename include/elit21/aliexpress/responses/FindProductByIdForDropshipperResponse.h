#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::aliexpress::responses {

class FindProductByIdForDropshipperResponse final : public platform::BusinessComponent {
public:
    FindProductByIdForDropshipperResponse();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::aliexpress::responses
