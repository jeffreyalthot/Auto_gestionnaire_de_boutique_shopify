#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::aliexpress::responses {

class MemberOrderDataSubmitResponse final : public platform::BusinessComponent {
public:
    MemberOrderDataSubmitResponse();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::aliexpress::responses
