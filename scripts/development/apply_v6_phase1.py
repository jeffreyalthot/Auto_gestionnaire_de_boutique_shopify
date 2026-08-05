from pathlib import Path

ROOT=Path('/mnt/data/ELIT21_Shopify_AliExpress_Autonomous_Manager-v6.0.0-SHOPIFY-TRANSACTIONAL')

def write(rel, content):
    p=ROOT/rel
    p.parent.mkdir(parents=True, exist_ok=True)
    p.write_text(content, encoding='utf-8')

def replace(rel, old, new, count=1):
    p=ROOT/rel
    text=p.read_text(encoding='utf-8')
    if old not in text:
        raise RuntimeError(f'needle missing in {rel}: {old[:100]!r}')
    p.write_text(text.replace(old,new,count), encoding='utf-8')

write('include/elit21/shopify/ShopifyMoney.h', r'''#pragma once

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
''')

write('src/elit21/shopify/ShopifyMoney.cpp', r'''#include "elit21/shopify/ShopifyMoney.h"

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
''')

write('include/elit21/shopify/ShopifyOrderStateMachine.h', r'''#pragma once
#include "elit21/core/Result.h"
#include <string>
#include <vector>
namespace elit21::shopify {
enum class ShopifyOrderState {received,validating,manual_review,pending_supplier,supplier_ordered,partially_fulfilled,fulfilled,cancellation_requested,cancelled,refund_received,refunded,failed,unknown};
class ShopifyOrderStateMachine {
public:
 static ShopifyOrderState parse(const std::string& value) noexcept;
 static std::string name(ShopifyOrderState state);
 static bool terminal(ShopifyOrderState state) noexcept;
 static bool canTransition(ShopifyOrderState from, ShopifyOrderState to) noexcept;
 static Result<void> validateTransition(const std::string& from,const std::string& to);
 static std::vector<std::string> allowedNext(const std::string& from);
};
}
''')

write('src/elit21/shopify/ShopifyOrderStateMachine.cpp', r'''#include "elit21/shopify/ShopifyOrderStateMachine.h"
#include "elit21/util/StringUtil.h"
#include <array>
#include <utility>
namespace elit21::shopify { namespace { using T=std::pair<ShopifyOrderState,ShopifyOrderState>; constexpr std::array<T,23> transitions{{
{ShopifyOrderState::received,ShopifyOrderState::validating},{ShopifyOrderState::received,ShopifyOrderState::manual_review},{ShopifyOrderState::received,ShopifyOrderState::cancelled},{ShopifyOrderState::validating,ShopifyOrderState::pending_supplier},{ShopifyOrderState::validating,ShopifyOrderState::manual_review},{ShopifyOrderState::validating,ShopifyOrderState::failed},{ShopifyOrderState::manual_review,ShopifyOrderState::pending_supplier},{ShopifyOrderState::manual_review,ShopifyOrderState::cancelled},{ShopifyOrderState::manual_review,ShopifyOrderState::failed},{ShopifyOrderState::pending_supplier,ShopifyOrderState::supplier_ordered},{ShopifyOrderState::pending_supplier,ShopifyOrderState::manual_review},{ShopifyOrderState::pending_supplier,ShopifyOrderState::cancellation_requested},{ShopifyOrderState::pending_supplier,ShopifyOrderState::failed},{ShopifyOrderState::supplier_ordered,ShopifyOrderState::partially_fulfilled},{ShopifyOrderState::supplier_ordered,ShopifyOrderState::fulfilled},{ShopifyOrderState::supplier_ordered,ShopifyOrderState::cancellation_requested},{ShopifyOrderState::supplier_ordered,ShopifyOrderState::refund_received},{ShopifyOrderState::partially_fulfilled,ShopifyOrderState::fulfilled},{ShopifyOrderState::partially_fulfilled,ShopifyOrderState::refund_received},{ShopifyOrderState::fulfilled,ShopifyOrderState::refund_received},{ShopifyOrderState::cancellation_requested,ShopifyOrderState::cancelled},{ShopifyOrderState::refund_received,ShopifyOrderState::refunded},{ShopifyOrderState::failed,ShopifyOrderState::manual_review}}; }
ShopifyOrderState ShopifyOrderStateMachine::parse(const std::string& v) noexcept { const auto n=util::lower(util::trim(v)); if(n=="received")return ShopifyOrderState::received;if(n=="validating")return ShopifyOrderState::validating;if(n=="manual_review"||n=="manual-review")return ShopifyOrderState::manual_review;if(n=="pending_supplier"||n=="dry_run_pending_supplier")return ShopifyOrderState::pending_supplier;if(n=="supplier_ordered")return ShopifyOrderState::supplier_ordered;if(n=="partially_fulfilled")return ShopifyOrderState::partially_fulfilled;if(n=="fulfilled"||n=="delivered")return ShopifyOrderState::fulfilled;if(n=="cancellation_requested")return ShopifyOrderState::cancellation_requested;if(n=="cancelled"||n=="canceled")return ShopifyOrderState::cancelled;if(n=="refund_received"||n=="refund_pending")return ShopifyOrderState::refund_received;if(n=="refunded")return ShopifyOrderState::refunded;if(n=="failed"||n=="dead")return ShopifyOrderState::failed;return ShopifyOrderState::unknown; }
std::string ShopifyOrderStateMachine::name(ShopifyOrderState s){switch(s){case ShopifyOrderState::received:return"received";case ShopifyOrderState::validating:return"validating";case ShopifyOrderState::manual_review:return"manual_review";case ShopifyOrderState::pending_supplier:return"pending_supplier";case ShopifyOrderState::supplier_ordered:return"supplier_ordered";case ShopifyOrderState::partially_fulfilled:return"partially_fulfilled";case ShopifyOrderState::fulfilled:return"fulfilled";case ShopifyOrderState::cancellation_requested:return"cancellation_requested";case ShopifyOrderState::cancelled:return"cancelled";case ShopifyOrderState::refund_received:return"refund_received";case ShopifyOrderState::refunded:return"refunded";case ShopifyOrderState::failed:return"failed";default:return"unknown";}}
bool ShopifyOrderStateMachine::terminal(ShopifyOrderState s) noexcept{return s==ShopifyOrderState::cancelled||s==ShopifyOrderState::refunded;}
bool ShopifyOrderStateMachine::canTransition(ShopifyOrderState f,ShopifyOrderState t) noexcept{if(f==t&&f!=ShopifyOrderState::unknown)return true;for(const auto&x:transitions)if(x.first==f&&x.second==t)return true;return false;}
Result<void> ShopifyOrderStateMachine::validateTransition(const std::string& f,const std::string& t){auto a=parse(f),b=parse(t);if(a==ShopifyOrderState::unknown)return Result<void>::failure("Unknown Shopify order source state: "+f);if(b==ShopifyOrderState::unknown)return Result<void>::failure("Unknown Shopify order destination state: "+t);if(!canTransition(a,b))return Result<void>::failure("Invalid Shopify order transition: "+name(a)+" -> "+name(b));return Result<void>::success();}
std::vector<std::string> ShopifyOrderStateMachine::allowedNext(const std::string& f){auto a=parse(f);std::vector<std::string>o;for(const auto&x:transitions)if(x.first==a)o.push_back(name(x.second));return o;}
}
''')

# Database header patches
replace('include/elit21/storage/Database.h', '''struct StoredOrder {
    long long id{0};
    std::string shopify_id, ae_id, status, payload, idempotency_key;
    double total{0};
};
''', '''struct StoredOrder {
    long long id{0};
    std::string shopify_id, ae_id, status, payload, idempotency_key;
    std::string customer_email;
    std::string currency{"CAD"};
    double total{0};
};

struct StoredOrderLine {
    long long id{0};
    long long order_id{0};
    std::string shopify_line_id, shopify_variant_id, aliexpress_product_id, aliexpress_sku_id, sku, title;
    int quantity{0};
    double unit_price_cad{0.0};
    std::string status{"pending"};
};

struct ShopifyPrivacyRequestRecord {
    std::string request_id, topic, shop_domain, customer_id;
    std::string orders_json{"[]"};
    std::string payload_hash;
    std::string status{"received"};
};
''')
replace('include/elit21/storage/Database.h', '''    Result<bool> saveOrder(const StoredOrder& order);
    Result<std::vector<StoredOrder>> ordersByStatus(const std::string& status, int limit = 100);
''', '''    Result<bool> saveOrder(const StoredOrder& order);
    Result<bool> saveOrderWithLines(const StoredOrder& order, const std::vector<StoredOrderLine>& lines);
    Result<std::vector<StoredOrderLine>> orderLines(const std::string& shopify_order_id);
    Result<std::vector<StoredOrder>> ordersByStatus(const std::string& status, int limit = 100);
    Result<std::vector<StoredOrder>> ordersByShopifyIds(const std::vector<std::string>& shopify_ids);
    Result<int> redactShopifyOrders(const std::vector<std::string>& shopify_ids,const std::string& reason);
    Result<int> redactAllShopifyCustomerData(const std::string& reason);
    Result<bool> transitionOrderStatus(const std::string& shopify_id,const std::vector<std::string>& expected_statuses,const std::string& next_status,const std::string& event_type,const std::string& context_json = "{}",const std::string& last_error = "");
''')
replace('include/elit21/storage/Database.h', '''    Result<void> setRuntimeState(const std::string& key, const std::string& value_json);
''', '''    Result<bool> acquireLease(const std::string& lease_name,const std::string& owner_id,int ttl_seconds);
    Result<void> releaseLease(const std::string& lease_name,const std::string& owner_id);
    Result<void> recordShopifyPrivacyRequest(const ShopifyPrivacyRequestRecord& request);

    Result<void> setRuntimeState(const std::string& key, const std::string& value_json);
''')

# Migration
write('migrations/0034_shopify_transactional_runtime.sql', r'''CREATE UNIQUE INDEX IF NOT EXISTS idx_order_lines_unique_shopify_line ON order_lines(order_id,shopify_line_item_id) WHERE shopify_line_item_id IS NOT NULL AND shopify_line_item_id<>'';
CREATE TABLE IF NOT EXISTS shopify_order_events(id INTEGER PRIMARY KEY AUTOINCREMENT,order_id INTEGER NOT NULL REFERENCES orders(id) ON DELETE CASCADE,event_type TEXT NOT NULL,from_status TEXT NOT NULL DEFAULT '',to_status TEXT NOT NULL,context_json TEXT NOT NULL DEFAULT '{}',created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP);
CREATE INDEX IF NOT EXISTS idx_shopify_order_events_order_created ON shopify_order_events(order_id,created_at);
CREATE TABLE IF NOT EXISTS shopify_sync_leases(lease_name TEXT PRIMARY KEY,owner_id TEXT NOT NULL,acquired_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,expires_at TEXT NOT NULL);
CREATE INDEX IF NOT EXISTS idx_shopify_sync_leases_expiry ON shopify_sync_leases(expires_at);
CREATE TABLE IF NOT EXISTS shopify_privacy_requests(id INTEGER PRIMARY KEY AUTOINCREMENT,request_id TEXT NOT NULL UNIQUE,topic TEXT NOT NULL,shop_domain TEXT NOT NULL,customer_id TEXT,order_ids_json TEXT NOT NULL DEFAULT '[]',payload_hash TEXT NOT NULL,status TEXT NOT NULL DEFAULT 'received',result_path TEXT,last_error TEXT,received_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,completed_at TEXT,updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP);
CREATE INDEX IF NOT EXISTS idx_shopify_privacy_requests_status ON shopify_privacy_requests(status,received_at);
CREATE TABLE IF NOT EXISTS shopify_installations(shop_domain TEXT PRIMARY KEY,api_version TEXT NOT NULL,access_scope TEXT NOT NULL DEFAULT '',token_fingerprint TEXT NOT NULL DEFAULT '',installed INTEGER NOT NULL DEFAULT 1 CHECK(installed IN (0,1)),installed_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,uninstalled_at TEXT,last_verified_at TEXT,metadata_json TEXT NOT NULL DEFAULT '{}');
CREATE TABLE IF NOT EXISTS shopify_api_version_observations(id INTEGER PRIMARY KEY AUTOINCREMENT,requested_version TEXT NOT NULL,served_version TEXT NOT NULL,operation_name TEXT NOT NULL DEFAULT '',request_id TEXT NOT NULL DEFAULT '',observed_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP);
CREATE INDEX IF NOT EXISTS idx_shopify_api_version_observations_time ON shopify_api_version_observations(observed_at);
''')

print('phase1 files written')
