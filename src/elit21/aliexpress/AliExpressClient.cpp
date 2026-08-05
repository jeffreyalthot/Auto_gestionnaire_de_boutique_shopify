#include "elit21/aliexpress/AliExpressClient.h"

#include "elit21/aliexpress/AliExpressMethodNames.h"
#include "elit21/aliexpress/auth/AliExpressAuthorizationUrlBuilder.h"
#include "elit21/security/Crypto.h"
#include "elit21/util/StringUtil.h"
#include "elit21/util/TimeUtil.h"

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <thread>

namespace elit21 {
namespace {

std::string firstNonEmpty(const Json& object,
                          std::initializer_list<const char*> keys,
                          const std::string& fallback = {}) {
    for (const auto* key : keys) {
        const auto value = object.getString(key);
        if (!value.empty()) return value;
    }
    return fallback;
}

int firstInt(const Json& object, std::initializer_list<const char*> keys, int fallback = 0) {
    for (const auto* key : keys) {
        if (object.contains(key)) return object.getInt(key, fallback);
    }
    return fallback;
}

double firstNumber(const Json& object,
                   std::initializer_list<const char*> keys,
                   double fallback = 0.0) {
    for (const auto* key : keys) {
        if (object.contains(key)) return object.getNumber(key, fallback);
    }
    return fallback;
}

void appendImageValues(const Json& value, std::vector<std::string>& images) {
    if (value.isArray()) {
        for (std::size_t index = 0; index < value.size(); ++index) {
            const auto url = value.at(index).stringValue();
            if (url.rfind("https://", 0) == 0) images.push_back(url);
        }
        return;
    }
    const auto text = value.stringValue();
    if (text.empty()) return;
    std::string current;
    for (char character : text) {
        if (character == ';' || character == ',') {
            current = util::trim(current);
            if (current.rfind("https://", 0) == 0) images.push_back(current);
            current.clear();
        } else {
            current.push_back(character);
        }
    }
    current = util::trim(current);
    if (current.rfind("https://", 0) == 0) images.push_back(current);
}

} // namespace

AliExpressClient::AliExpressClient(AliExpressConfig config, HttpClient& http)
    : config_(std::move(config)), http_(http) {}

Json AliExpressClient::responseResult(const Json& root, const std::string& response_key) {
    auto response = root.get(response_key);
    if (!response.isNull()) {
        auto result = response.get("result");
        return result.isNull() ? response : result;
    }
    auto result = root.get("result");
    return result.isNull() ? root : result;
}

double AliExpressClient::parseRate(const Json& object, const std::string& key, double fallback) {
    if (!object.contains(key)) return fallback;
    const auto text = object.getString(key);
    if (!text.empty()) {
        std::string normalized = text;
        normalized.erase(std::remove(normalized.begin(), normalized.end(), '%'), normalized.end());
        try { return std::stod(normalized); } catch (...) { return fallback; }
    }
    return object.getNumber(key, fallback);
}

Result<Json> AliExpressClient::call(const std::string& method,
                                    std::map<std::string, std::string> parameters,
                                    bool requires_session,
                                    int max_attempts) {
    if (!aliexpress::AliExpressMethodNames::supported(method)) {
        return Result<Json>::failure("Unsupported AliExpress method: " + method);
    }
    if (config_.app_key.empty() || config_.app_secret.empty()) {
        return Result<Json>::failure("AliExpress app_key/app_secret are missing");
    }
    if (requires_session && config_.access_token.empty()) {
        return Result<Json>::failure("AliExpress access token is required for " + method);
    }
    if (config_.gateway.rfind("https://", 0) != 0) {
        return Result<Json>::failure("AliExpress gateway must use HTTPS");
    }

    parameters["method"] = method;
    parameters["app_key"] = config_.app_key;
    parameters["timestamp"] = util::localNowDisplay();
    parameters["format"] = "json";
    parameters["v"] = "2.0";
    parameters["sign_method"] = "md5";
    parameters["simplify"] = "true";
    if (requires_session) parameters["session"] = config_.access_token;
    parameters["sign"] = crypto::aliExpressTopSign(parameters, config_.app_secret);

    std::string last_error;
    for (int attempt = 0; attempt < std::max(1, max_attempts); ++attempt) {
        auto response = http_.postForm(config_.gateway, parameters, {{"Accept", "application/json"}});
        if (!response) {
            last_error = response.error();
        } else if (response.value().status == 429 || response.value().status >= 500) {
            last_error = "AliExpress transient HTTP " + std::to_string(response.value().status);
        } else if (response.value().status < 200 || response.value().status >= 300) {
            return Result<Json>::failure(
                "AliExpress HTTP " + std::to_string(response.value().status) + ": " + response.value().body);
        } else {
            auto parsed = Json::parse(response.value().body);
            if (!parsed) return Result<Json>::failure(parsed.error());
            if (parsed.value().contains("error_response")) {
                return Result<Json>::failure("AliExpress: " + parsed.value().get("error_response").dump());
            }
            return parsed;
        }
        if (attempt + 1 < max_attempts) {
            std::this_thread::sleep_for(std::chrono::milliseconds(250 * (1 << attempt)));
        }
    }
    return Result<Json>::failure(last_error.empty() ? "AliExpress request failed" : last_error);
}

Result<void> AliExpressClient::healthCheck() {
    auto response = recommendations(1, 1);
    return response ? Result<void>::success() : Result<void>::failure(response.error());
}

Result<AliProduct> AliExpressClient::getProduct(const std::string& product_id) {
    if (product_id.empty()) return Result<AliProduct>::failure("AliExpress product ID is empty");
    std::map<std::string, std::string> parameters{
        {"product_id", product_id},
        {"ship_to_country", config_.country},
        {"target_currency", config_.currency},
        {"target_language", config_.language}
    };
    auto response = call(aliexpress::AliExpressMethodNames::productGet(), parameters, true);
    if (!response) return Result<AliProduct>::failure(response.error());
    const auto data = responseResult(response.value(), "aliexpress_ds_product_get_response");

    AliProduct product;
    product.product_id = firstNonEmpty(data, {"product_id"}, product_id);
    product.title = firstNonEmpty(data, {"product_title", "subject", "title"});
    product.description = firstNonEmpty(data, {"detail", "product_description", "description"});
    product.min_price_cad = firstNumber(data, {"min_sale_price", "target_sale_price", "min_price"});
    product.max_price_cad = firstNumber(data, {"max_sale_price", "target_original_price", "max_price"}, product.min_price_cad);
    product.rating = parseRate(data, "evaluate_rate", firstNumber(data, {"rating"}));
    product.orders = firstInt(data, {"lastest_volume", "orders", "sale_count"});
    product.stock = firstInt(data, {"total_available_stock", "stock", "inventory"});
    product.seller_id = firstNonEmpty(data, {"seller_id", "store_id"});
    product.seller_name = firstNonEmpty(data, {"store_name", "seller_name"});
    product.seller_score = parseRate(data, "positive_rate", firstNumber(data, {"seller_score"}));

    appendImageValues(data.get("ae_multimedia_info_dto").get("image_urls"), product.images);
    appendImageValues(data.get("image_urls"), product.images);
    appendImageValues(data.get("product_main_image_url"), product.images);
    std::sort(product.images.begin(), product.images.end());
    product.images.erase(std::unique(product.images.begin(), product.images.end()), product.images.end());

    auto skus = data.get("ae_item_sku_info_dtos");
    if (skus.isObject()) skus = skus.get("ae_item_sku_info_d_t_o");
    for (std::size_t index = 0; index < skus.size(); ++index) {
        const auto item = skus.at(index);
        AliProductSku sku;
        sku.sku_id = firstNonEmpty(item, {"sku_id", "id", "sku_attr"});
        sku.sku_code = firstNonEmpty(item, {"sku_code", "sku_code_str"});
        sku.price_cad = firstNumber(item, {"sku_price", "offer_sale_price", "price"}, product.min_price_cad);
        sku.stock = firstInt(item, {"sku_available_stock", "stock"}, product.stock);
        product.skus.push_back(std::move(sku));
    }
    return Result<AliProduct>::success(std::move(product));
}

Result<std::vector<AliProduct>> AliExpressClient::recommendations(int page, int page_size) {
    std::map<std::string, std::string> parameters{
        {"page_no", std::to_string(std::max(1, page))},
        {"page_size", std::to_string(std::max(1, std::min(page_size, 50)))},
        {"country", config_.country},
        {"target_currency", config_.currency},
        {"target_language", config_.language}
    };
    auto response = call(aliexpress::AliExpressMethodNames::recommendFeed(), parameters, false);
    if (!response) return Result<std::vector<AliProduct>>::failure(response.error());
    auto data = responseResult(response.value(), "aliexpress_ds_recommend_feed_get_response");
    auto products = data.get("products");
    if (products.isObject()) products = products.get("traffic_product_d_t_o");
    if (products.isNull() && data.isArray()) products = data;
    std::vector<AliProduct> output;
    for (std::size_t index = 0; index < products.size(); ++index) {
        const auto item = products.at(index);
        AliProduct product;
        product.product_id = firstNonEmpty(item, {"product_id", "item_id"});
        product.title = firstNonEmpty(item, {"product_title", "title"});
        product.min_price_cad = firstNumber(item, {"target_sale_price", "sale_price", "price"});
        product.max_price_cad = firstNumber(item, {"target_original_price", "original_price"}, product.min_price_cad);
        product.rating = parseRate(item, "evaluate_rate", firstNumber(item, {"rating"}));
        product.orders = firstInt(item, {"lastest_volume", "orders"});
        appendImageValues(item.get("product_main_image_url"), product.images);
        if (!product.product_id.empty()) output.push_back(std::move(product));
    }
    return Result<std::vector<AliProduct>>::success(std::move(output));
}

Result<std::vector<FreightQuote>> AliExpressClient::calculateFreight(const std::string& product_id,
                                                                     const std::string& sku_id,
                                                                     int quantity,
                                                                     const Address& address) {
    if (product_id.empty() || quantity <= 0) {
        return Result<std::vector<FreightQuote>>::failure("Invalid freight request");
    }
    Json query = Json::object();
    query.set("product_id", product_id);
    query.set("selected_sku_id", sku_id);
    query.set("product_num", quantity);
    query.set("country", config_.country);
    query.set("province_code", address.province_code);
    query.set("city", address.city);
    query.set("currency", config_.currency);
    std::map<std::string, std::string> parameters{
        {"param_aeop_freight_calculate_for_buyer_d_t_o", query.dump()}
    };
    auto response = call(aliexpress::AliExpressMethodNames::freightCalculate(), parameters, true);
    if (!response) return Result<std::vector<FreightQuote>>::failure(response.error());
    auto list = responseResult(response.value(), "aliexpress_logistics_buyer_freight_calculate_response");
    if (list.isObject()) {
        auto nested = list.get("aeop_freight_calculate_result_for_buyer_d_t_o_list");
        if (!nested.isNull()) list = nested;
    }
    std::vector<FreightQuote> output;
    for (std::size_t index = 0; index < list.size(); ++index) {
        const auto item = list.at(index);
        FreightQuote quote;
        quote.service_name = firstNonEmpty(item, {"service_name", "logistics_service_name"});
        quote.service_code = firstNonEmpty(item, {"service_code", "service_name"}, quote.service_name);
        quote.amount_cad = firstNumber(item, {"freight_amount", "amount", "shipping_fee"});
        quote.estimated_days = firstInt(item, {"estimated_delivery_time", "delivery_days"}, 45);
        quote.available = item.getBool("available", true);
        quote.tracking_available = item.getBool("tracking_available", !quote.service_code.empty());
        output.push_back(std::move(quote));
    }
    std::sort(output.begin(), output.end(), [](const auto& left, const auto& right) {
        if (left.available != right.available) return left.available > right.available;
        if (left.estimated_days != right.estimated_days) return left.estimated_days < right.estimated_days;
        return left.amount_cad < right.amount_cad;
    });
    return Result<std::vector<FreightQuote>>::success(std::move(output));
}

Result<SupplierOrderResult> AliExpressClient::placeOrder(const CustomerOrder& order,
                                                          const std::string& service) {
    if (order.lines.empty()) return Result<SupplierOrderResult>::failure("Supplier order has no lines");
    Json address = Json::object();
    address.set("contact_person", order.shipping_address.first_name + " " + order.shipping_address.last_name);
    address.set("country", config_.country);
    address.set("province", order.shipping_address.province);
    address.set("city", order.shipping_address.city);
    address.set("address", order.shipping_address.address1 +
        (order.shipping_address.address2.empty() ? "" : " " + order.shipping_address.address2));
    address.set("zip", order.shipping_address.postal_code);
    address.set("mobile_no", order.shipping_address.phone);

    Json items = Json::array();
    for (const auto& line : order.lines) {
        if (line.aliexpress_product_id.empty() || line.aliexpress_sku_id.empty() || line.quantity <= 0) {
            return Result<SupplierOrderResult>::failure("Supplier order line mapping is incomplete");
        }
        Json item = Json::object();
        item.set("product_id", line.aliexpress_product_id);
        item.set("sku_attr", line.aliexpress_sku_id);
        item.set("product_count", line.quantity);
        item.set("logistics_service_name", service);
        items.push(item);
    }
    Json input = Json::object();
    input.set("logistics_address", address);
    input.set("product_items", items);
    std::map<std::string, std::string> parameters{
        {"param_place_order_request4_open_api_d_t_o", input.dump()}
    };
    auto response = call(aliexpress::AliExpressMethodNames::placeOrder(), parameters, true);
    if (!response) return Result<SupplierOrderResult>::failure(response.error());
    const auto data = responseResult(response.value(), "aliexpress_trade_buy_placeorder_response");
    SupplierOrderResult result;
    result.status = data.getBool("is_success", false) ? "PLACED" : firstNonEmpty(data, {"status"}, "FAILED");
    result.amount = firstNumber(data, {"total_amount", "amount"});
    result.raw_json = data.dump();
    auto order_list = data.get("order_list");
    if (order_list.isArray()) {
        for (std::size_t index = 0; index < order_list.size(); ++index) {
            auto id = order_list.at(index).stringValue();
            if (id.empty() && order_list.at(index).isObject()) {
                id = firstNonEmpty(order_list.at(index), {"order_id", "id"});
            }
            if (!id.empty()) result.order_ids.push_back(id);
        }
    } else {
        const auto text = order_list.stringValue();
        if (!text.empty()) result.order_ids.push_back(text);
    }
    if (result.order_ids.empty()) {
        const auto id = firstNonEmpty(data, {"order_id", "orderId"});
        if (!id.empty()) result.order_ids.push_back(id);
    }
    if (!result.order_ids.empty()) result.order_id = result.order_ids.front();
    if (result.order_id.empty()) {
        return Result<SupplierOrderResult>::failure(
            "AliExpress returned no supplier order ID: " + data.dump());
    }
    return Result<SupplierOrderResult>::success(std::move(result));
}

Result<Json> AliExpressClient::getOrder(const std::string& order_id) {
    if (order_id.empty()) return Result<Json>::failure("AliExpress order ID is empty");
    auto modern = call(aliexpress::AliExpressMethodNames::orderGet(), {{"order_id", order_id}}, true);
    if (modern) return modern;
    Json query = Json::object();
    query.set("order_id", order_id);
    auto legacy = call("aliexpress.trade.ds.order.get", {{"single_order_query", query.dump()}}, true);
    return legacy ? legacy : Result<Json>::failure(modern.error() + "; fallback: " + legacy.error());
}

Result<ShipmentInfo> AliExpressClient::getTracking(const std::string& order_id,
                                                    const std::string& tracking_number,
                                                    const std::string& service) {
    auto response = call(
        aliexpress::AliExpressMethodNames::trackingQuery(),
        {{"order_id", order_id},
         {"logistics_no", tracking_number},
         {"service_name", service},
         {"to_area", config_.country}},
        true);
    if (!response) return Result<ShipmentInfo>::failure(response.error());
    auto data = responseResult(response.value(), "aliexpress_logistics_ds_trackinginfo_query_response");
    if (data.isObject() && data.contains("details")) data = data.get("details");
    ShipmentInfo shipment;
    shipment.tracking_number = tracking_number;
    shipment.carrier = service;
    if (data.isArray() && data.size() > 0) {
        const auto last = data.at(data.size() - 1);
        shipment.status = firstNonEmpty(last, {"status", "event_status"}, "IN_TRANSIT");
        shipment.last_event = last.dump();
    } else {
        shipment.status = firstNonEmpty(data, {"status", "event_status"}, "IN_TRANSIT");
        shipment.last_event = data.dump();
    }
    return Result<ShipmentInfo>::success(std::move(shipment));
}

std::string AliExpressClient::authorizationUrl(const std::string& redirect_uri,
                                                const std::string& state) const {
    return aliexpress::auth::AliExpressAuthorizationUrlBuilder::build(
        config_.app_key, redirect_uri, state, true);
}

} // namespace elit21
