#pragma once

#include "elit21/core/Result.h"
#include "elit21/platform/BusinessComponent.h"

#include <string>

namespace elit21::security {

class DataEncryptor final : public platform::BusinessComponent {
public:
    DataEncryptor();
    platform::OperationResult execute(const platform::OperationContext& context) override;

    static Result<std::string> encrypt(const std::string& plaintext,
                                       const std::string& master_secret,
                                       const std::string& associated_data = "ELIT21");
    static Result<std::string> decrypt(const std::string& envelope,
                                       const std::string& master_secret,
                                       const std::string& associated_data = "ELIT21");
};

} // namespace elit21::security
