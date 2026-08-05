#pragma once

#include "elit21/platform/ManagedComponent.h"
#include <cstddef>
#include <string>
#include <vector>

namespace elit21::platform {

struct BusinessComponentSpec {
    std::string domain;
    std::string operation;
    std::vector<std::string> required_fields;
    bool mutating{false};
    bool requires_credentials{false};
    std::size_t max_payload_bytes{4U * 1024U * 1024U};
};

class BusinessComponent : public ManagedComponent {
public:
    BusinessComponent(std::string name, std::string role, BusinessComponentSpec spec);

protected:
    [[nodiscard]] OperationResult executeBusiness(const OperationContext& context) const;
    [[nodiscard]] const BusinessComponentSpec& businessSpec() const noexcept { return spec_; }

private:
    BusinessComponentSpec spec_;
};

} // namespace elit21::platform
