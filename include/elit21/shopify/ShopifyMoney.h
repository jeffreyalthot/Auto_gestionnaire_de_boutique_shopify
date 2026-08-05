#pragma once

#include "elit21/core/Result.h"
#include "elit21/json/Json.h"

#include <compare>
#include <cstdint>
#include <string>

namespace elit21::shopify {

class ShopifyMoney {
public:
    ShopifyMoney() = default;
    explicit ShopifyMoney(std::int64_t cents) : cents_(cents) {}

    static Result<ShopifyMoney> parse(const std::string& decimal);
    static Result<ShopifyMoney> fromDouble(double amount);
    static Result<ShopifyMoney> supplierPrice(ShopifyMoney supplier_cost,
                                               double markup_percent_before_shipping,
                                               ShopifyMoney shipping);

    [[nodiscard]] std::int64_t cents() const noexcept { return cents_; }
    [[nodiscard]] double toDouble() const noexcept;
    [[nodiscard]] std::string toDecimal() const;
    [[nodiscard]] Json toJson(const std::string& currency = "CAD") const;
    [[nodiscard]] bool nonNegative() const noexcept { return cents_ >= 0; }

    ShopifyMoney& operator+=(ShopifyMoney other) noexcept;
    friend ShopifyMoney operator+(ShopifyMoney left, ShopifyMoney right) noexcept { left += right; return left; }
    friend ShopifyMoney operator-(ShopifyMoney left, ShopifyMoney right) noexcept { return ShopifyMoney(left.cents_ - right.cents_); }
    friend bool operator==(ShopifyMoney left, ShopifyMoney right) noexcept { return left.cents_ == right.cents_; }
    friend auto operator<=>(ShopifyMoney left, ShopifyMoney right) noexcept = default;

private:
    std::int64_t cents_{0};
};

} // namespace elit21::shopify
