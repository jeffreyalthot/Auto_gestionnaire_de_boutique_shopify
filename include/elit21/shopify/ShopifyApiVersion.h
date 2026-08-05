#pragma once

#include "elit21/core/Result.h"

#include <compare>
#include <string>
#include <vector>

namespace elit21::shopify {

class ShopifyApiVersion {
public:
    static constexpr const char* latestStable() noexcept { return "2026-07"; }

    explicit ShopifyApiVersion(std::string value = latestStable());

    [[nodiscard]] const std::string& value() const noexcept { return value_; }
    [[nodiscard]] int year() const noexcept;
    [[nodiscard]] int month() const noexcept;
    [[nodiscard]] int quarter() const noexcept;
    [[nodiscard]] bool valid() const noexcept;
    [[nodiscard]] bool isReleaseCandidate() const noexcept;
    [[nodiscard]] std::string graphqlPath() const;
    [[nodiscard]] std::string storefrontPath() const;
    [[nodiscard]] ShopifyApiVersion nextQuarter() const;
    [[nodiscard]] ShopifyApiVersion previousQuarter() const;

    static Result<ShopifyApiVersion> parse(const std::string& value);
    static std::vector<ShopifyApiVersion> supportedWindow(const ShopifyApiVersion& newest,
                                                           int releases = 8);

    friend bool operator==(const ShopifyApiVersion&, const ShopifyApiVersion&) = default;
    friend std::strong_ordering operator<=>(const ShopifyApiVersion& left,
                                             const ShopifyApiVersion& right) noexcept;

private:
    std::string value_;
};

} // namespace elit21::shopify
