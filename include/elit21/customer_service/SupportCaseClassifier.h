#pragma once

#include "elit21/platform/BusinessComponent.h"

namespace elit21::customer_service {

class SupportCaseClassifier final : public platform::BusinessComponent {
public:
    SupportCaseClassifier();
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::customer_service
