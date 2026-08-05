#pragma once

#include "elit21/platform/BusinessComponent.h"
#include <string>

namespace elit21::security {
class DataRedactor final : public platform::BusinessComponent {
public:
    DataRedactor();
    platform::OperationResult execute(const platform::OperationContext& context) override;
    static std::string redact(const std::string& text);
};
}
