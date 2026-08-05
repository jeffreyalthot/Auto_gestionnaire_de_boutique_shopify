#include "elit21/shopify/ShopifyMoney.h"

#include "elit21/util/StringUtil.h"

#include <cmath>
#include <iomanip>
#include <limits>
#include <sstream>

namespace elit21::shopify {
namespace {
bool asciiDigit(char value) noexcept { return value >= '0' && value <= '9'; }
}

Result<ShopifyMoney> ShopifyMoney::parse(const std::string& decimal) {
    const auto value = util::trim(decimal);
    if (value.empty()) return Result<ShopifyMoney>::failure("Money value is empty");
    std::size_t position = 0;
    bool negative = false;
    if (value[position] == '+' || value[position] == '-') { negative = value[position] == '-'; ++position; }
    if (position >= value.size()) return Result<ShopifyMoney>::failure("Money value has no digits");
    std::int64_t whole = 0;
    bool has_whole_digit = false;
    while (position < value.size() && asciiDigit(value[position])) {
        has_whole_digit = true;
        const auto digit = static_cast<std::int64_t>(value[position] - '0');
        if (whole > (std::numeric_limits<std::int64_t>::max() - digit) / 10)
            return Result<ShopifyMoney>::failure("Money value is too large");
        whole = whole * 10 + digit;
        ++position;
    }
    if (!has_whole_digit) return Result<ShopifyMoney>::failure("Money value has invalid whole amount");
    int first_decimal = 0, second_decimal = 0, rounding_digit = 0;
    if (position < value.size() && value[position] == '.') {
        ++position;
        if (position < value.size() && asciiDigit(value[position])) first_decimal = value[position++] - '0';
        if (position < value.size() && asciiDigit(value[position])) second_decimal = value[position++] - '0';
        if (position < value.size() && asciiDigit(value[position])) rounding_digit = value[position++] - '0';
        while (position < value.size() && asciiDigit(value[position])) ++position;
    }
    if (position != value.size()) return Result<ShopifyMoney>::failure("Money value contains invalid characters");
    if (whole > std::numeric_limits<std::int64_t>::max() / 100)
        return Result<ShopifyMoney>::failure("Money value is too large");
    std::int64_t cents = whole * 100 + first_decimal * 10 + second_decimal;
    if (rounding_digit >= 5) ++cents;
    if (negative) cents = -cents;
    return Result<ShopifyMoney>::success(ShopifyMoney(cents));
}

Result<ShopifyMoney> ShopifyMoney::fromDouble(double amount) {
    if (!std::isfinite(amount)) return Result<ShopifyMoney>::failure("Money amount is not finite");
    const long double scaled = static_cast<long double>(amount) * 100.0L;
    if (scaled > static_cast<long double>(std::numeric_limits<std::int64_t>::max()) ||
        scaled < static_cast<long double>(std::numeric_limits<std::int64_t>::min()))
        return Result<ShopifyMoney>::failure("Money amount is out of range");
    return Result<ShopifyMoney>::success(ShopifyMoney(static_cast<std::int64_t>(std::llround(scaled))));
}

Result<ShopifyMoney> ShopifyMoney::supplierPrice(ShopifyMoney supplier_cost,
                                                  double markup_percent_before_shipping,
                                                  ShopifyMoney shipping) {
    if (!supplier_cost.nonNegative() || !shipping.nonNegative())
        return Result<ShopifyMoney>::failure("Supplier cost and shipping must be non-negative");
    if (!std::isfinite(markup_percent_before_shipping) || markup_percent_before_shipping < 100.0)
        return Result<ShopifyMoney>::failure("Markup before shipping must be at least 100 percent");
    const long double multiplier = 1.0L + static_cast<long double>(markup_percent_before_shipping) / 100.0L;
    const long double marked_up = static_cast<long double>(supplier_cost.cents_) * multiplier;
    if (marked_up > static_cast<long double>(std::numeric_limits<std::int64_t>::max()))
        return Result<ShopifyMoney>::failure("Calculated selling price is out of range");
    const auto before_shipping = static_cast<std::int64_t>(std::ceil(marked_up - 1e-12L));
    if (shipping.cents_ > std::numeric_limits<std::int64_t>::max() - before_shipping)
        return Result<ShopifyMoney>::failure("Calculated final price is out of range");
    return Result<ShopifyMoney>::success(ShopifyMoney(before_shipping + shipping.cents_));
}

double ShopifyMoney::toDouble() const noexcept { return static_cast<double>(cents_) / 100.0; }

std::string ShopifyMoney::toDecimal() const {
    const bool negative = cents_ < 0;
    const std::uint64_t magnitude = negative ? static_cast<std::uint64_t>(-(cents_ + 1)) + 1U : static_cast<std::uint64_t>(cents_);
    std::ostringstream output;
    if (negative) output << '-';
    output << magnitude / 100U << '.' << std::setw(2) << std::setfill('0') << magnitude % 100U;
    return output.str();
}

Json ShopifyMoney::toJson(const std::string& currency) const {
    Json output = Json::object();
    output.set("amount", toDecimal());
    output.set("currencyCode", currency);
    output.set("cents", cents_);
    return output;
}

ShopifyMoney& ShopifyMoney::operator+=(ShopifyMoney other) noexcept { cents_ += other.cents_; return *this; }

} // namespace elit21::shopify
