#pragma once

#include "elit21/core/Result.h"

#include <cstdint>
#include <map>
#include <optional>
#include <string>

namespace elit21::shopify {

class ShopifyGlobalId {
public:
    ShopifyGlobalId() = default;

    static Result<ShopifyGlobalId> parse(const std::string& value);
    static Result<ShopifyGlobalId> fromNumeric(const std::string& resource_type,
                                               std::uint64_t numeric_id);

    [[nodiscard]] bool valid() const noexcept;
    [[nodiscard]] const std::string& resourceType() const noexcept { return resource_type_; }
    [[nodiscard]] const std::string& resourceId() const noexcept { return resource_id_; }
    [[nodiscard]] const std::map<std::string, std::string>& parameters() const noexcept {
        return parameters_;
    }
    [[nodiscard]] std::optional<std::uint64_t> numericId() const noexcept;
    [[nodiscard]] std::string toString() const;
    [[nodiscard]] bool isType(const std::string& expected) const noexcept;

private:
    std::string resource_type_;
    std::string resource_id_;
    std::map<std::string, std::string> parameters_;
};

} // namespace elit21::shopify
