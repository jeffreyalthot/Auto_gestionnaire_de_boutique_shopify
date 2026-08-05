#include "elit21/shopify/ShopifyWebhook.h"

#include "elit21/json/Json.h"
#include "elit21/security/Crypto.h"
#include "elit21/util/StringUtil.h"

#include <cerrno>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <string>

namespace elit21 {
namespace {

Result<double> decimalValue(const Json& value, const std::string& field) {
    if (value.isNumber()) {
        const double number = value.numberValue(std::numeric_limits<double>::quiet_NaN());
        if (std::isfinite(number)) return Result<double>::success(number);
        return Result<double>::failure("Valeur numérique non finie pour " + field);
    }
    if (!value.isString()) return Result<double>::failure("Valeur numérique manquante pour " + field);

    const auto text = util::trim(value.stringValue());
    if (text.empty()) return Result<double>::failure("Valeur numérique vide pour " + field);
    errno = 0;
    char* end = nullptr;
    const double number = std::strtod(text.c_str(), &end);
    if (errno == ERANGE || end == text.c_str() || *end != '\0' || !std::isfinite(number)) {
        return Result<double>::failure("Valeur numérique invalide pour " + field + ": " + text);
    }
    return Result<double>::success(number);
}

Result<double> moneyField(const Json& object,
                          const std::string& primary,
                          const std::string& fallback = {}) {
    if (object.contains(primary)) return decimalValue(object.get(primary), primary);
    if (!fallback.empty() && object.contains(fallback)) return decimalValue(object.get(fallback), fallback);
    return Result<double>::failure("Montant requis absent: " + primary);
}

std::string scalarId(const Json& object, const std::string& key) {
    const auto value = object.getScalarString(key);
    return value == "0" ? std::string{} : value;
}

void parseAliExpressProperties(const Json& properties, OrderLine& line) {
    if (!properties.isArray()) return;
    for (std::size_t index = 0; index < properties.size(); ++index) {
        const auto property = properties.at(index);
        const auto name = util::lower(util::trim(property.getString("name")));
        const auto value = util::trim(property.getScalarString("value"));
        if (name == "_aliexpress_product_id" || name == "aliexpress_product_id" || name == "ae_product_id") {
            line.aliexpress_product_id = value;
        } else if (name == "_aliexpress_sku_id" || name == "aliexpress_sku_id" || name == "ae_sku_id") {
            line.aliexpress_sku_id = value;
        }
    }
}

} // namespace

bool ShopifyWebhook::verify(const IncomingRequest& request) const {
    const auto iterator = request.headers.find("x-shopify-hmac-sha256");
    if (iterator == request.headers.end() || config_.webhook_secret.empty()) return false;
    const auto expected = crypto::hmacSha256Base64(config_.webhook_secret, request.body);
    return crypto::constantTimeEquals(util::trim(iterator->second), expected);
}

Result<CustomerOrder> ShopifyWebhook::parseOrderPaid(const std::string& body) const {
    if (body.empty()) return Result<CustomerOrder>::failure("Payload Shopify vide.");
    auto parsed = Json::parse(body);
    if (!parsed) return Result<CustomerOrder>::failure("JSON Shopify invalide: " + parsed.error());
    const auto root = parsed.value();
    if (!root.isObject()) return Result<CustomerOrder>::failure("Le webhook Shopify doit contenir un objet JSON.");

    CustomerOrder order;
    order.shopify_order_id = scalarId(root, "admin_graphql_api_id");
    if (order.shopify_order_id.empty()) order.shopify_order_id = scalarId(root, "id");
    if (order.shopify_order_id.empty()) return Result<CustomerOrder>::failure("Identifiant de commande Shopify manquant.");

    order.order_name = root.getString("name");
    order.email = root.getString("email");
    order.phone = root.getString("phone");
    order.currency = root.getString("currency", "CAD");
    auto total = moneyField(root, "current_total_price", "total_price");
    if (!total) return Result<CustomerOrder>::failure(total.error());
    if (total.value() <= 0.0) return Result<CustomerOrder>::failure("Le total Shopify doit être positif.");
    order.total_cad = total.value();

    const auto address = root.get("shipping_address");
    if (!address.isObject()) return Result<CustomerOrder>::failure("Adresse de livraison Shopify absente.");
    order.shipping_address.first_name = address.getString("first_name");
    order.shipping_address.last_name = address.getString("last_name");
    order.shipping_address.company = address.getString("company");
    order.shipping_address.address1 = address.getString("address1");
    order.shipping_address.address2 = address.getString("address2");
    order.shipping_address.city = address.getString("city");
    order.shipping_address.province = address.getString("province");
    order.shipping_address.province_code = address.getString("province_code");
    order.shipping_address.country = address.getString("country", "Canada");
    order.shipping_address.country_code = address.getString("country_code", "CA");
    order.shipping_address.postal_code = address.getString("zip");
    order.shipping_address.phone = address.getString("phone", order.phone);

    const auto lines = root.get("line_items");
    if (!lines.isArray() || lines.size() == 0) {
        return Result<CustomerOrder>::failure("La commande Shopify ne contient aucune ligne.");
    }
    order.lines.reserve(lines.size());
    for (std::size_t index = 0; index < lines.size(); ++index) {
        const auto value = lines.at(index);
        if (!value.isObject()) return Result<CustomerOrder>::failure("Ligne Shopify invalide à l’index " + std::to_string(index));

        OrderLine line;
        line.shopify_line_id = scalarId(value, "admin_graphql_api_id");
        if (line.shopify_line_id.empty()) line.shopify_line_id = scalarId(value, "id");
        line.shopify_variant_id = scalarId(value, "variant_id");
        line.sku = util::trim(value.getString("sku"));
        line.title = value.getString("title");
        line.quantity = value.getInt("quantity", 0);
        if (line.quantity <= 0) {
            return Result<CustomerOrder>::failure("Quantité Shopify invalide à l’index " + std::to_string(index));
        }
        if (value.contains("price")) {
            auto price = decimalValue(value.get("price"), "line_items.price");
            if (!price) return Result<CustomerOrder>::failure(price.error());
            line.unit_price_cad = price.value();
        }
        parseAliExpressProperties(value.get("properties"), line);
        order.lines.push_back(std::move(line));
    }

    order.raw_json = body;
    return Result<CustomerOrder>::success(std::move(order));
}

} // namespace elit21
