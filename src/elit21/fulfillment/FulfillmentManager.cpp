#include "elit21/fulfillment/FulfillmentManager.h"

#include "elit21/json/Json.h"

#include <algorithm>
#include <cctype>
#include <json-c/json.h>
#include <string>

namespace elit21 {
namespace {

std::string normalizeShopifyOrderId(const std::string& value) {
    if (value.rfind("gid://shopify/Order/", 0) == 0) return value;
    if (!value.empty() && std::all_of(value.begin(), value.end(), [](unsigned char c) { return std::isdigit(c); })) {
        return "gid://shopify/Order/" + value;
    }
    return value;
}

std::string findStringRecursive(json_object* object, const std::vector<std::string>& keys, int depth = 0) {
    if (object == nullptr || depth > 12) return {};
    const auto type = json_object_get_type(object);
    if (type == json_type_object) {
        for (const auto& key : keys) {
            json_object* value = nullptr;
            if (json_object_object_get_ex(object, key.c_str(), &value) && value != nullptr) {
                if (json_object_get_type(value) == json_type_string) return json_object_get_string(value);
                if (json_object_get_type(value) == json_type_int) return std::to_string(json_object_get_int64(value));
            }
        }
        json_object_object_foreach(object, key, value) {
            (void)key;
            const auto result = findStringRecursive(value, keys, depth + 1);
            if (!result.empty()) return result;
        }
    } else if (type == json_type_array) {
        const auto count = json_object_array_length(object);
        for (std::size_t index = 0; index < count; ++index) {
            const auto result = findStringRecursive(json_object_array_get_idx(object, index), keys, depth + 1);
            if (!result.empty()) return result;
        }
    }
    return {};
}

bool isDelivered(std::string status) {
    std::transform(status.begin(), status.end(), status.begin(), [](unsigned char c) {
        return static_cast<char>(std::toupper(c));
    });
    return status.find("DELIVER") != std::string::npos || status.find("SIGN") != std::string::npos;
}

} // namespace

void FulfillmentManager::synchronize() {
    auto pending = db_.ordersByStatus("supplier_ordered", 50);
    if (!pending) {
        ++counters_.errors;
        log_.error("fulfillment", pending.error());
        return;
    }

    for (const auto& order : pending.value()) {
        if (order.ae_id.rfind("DRYRUN-", 0) == 0) {
            ShipmentInfo shipment{
                "DRY-TRACK-" + order.shopify_id,
                "ELIT21 Simulation",
                "IN_TRANSIT",
                "Expédition simulée en mode dry-run"
            };
            auto saved = db_.saveShipment(order.id, shipment);
            auto updated = db_.updateOrderSupplierId(order.shopify_id, order.ae_id, "shipped");
            if (!saved || !updated) {
                ++counters_.errors;
                log_.error("fulfillment", !saved ? saved.error() : updated.error());
                continue;
            }
            ++counters_.shipped;
            db_.audit("INFO", "fulfillment", "Expédition simulée", "{}", order.shopify_id);
            continue;
        }

        auto details = ali_.getOrder(order.ae_id);
        if (!details) {
            ++counters_.errors;
            log_.warning("fulfillment", order.ae_id + ": " + details.error());
            continue;
        }

        const std::string tracking_number = findStringRecursive(
            details.value().raw(), {"tracking_number", "logistics_no", "logisticsNo", "mail_no"});
        const std::string logistics_service = findStringRecursive(
            details.value().raw(), {"logistics_service_name", "logisticsServiceName", "service_name", "carrier"});
        if (tracking_number.empty()) {
            log_.debug("fulfillment", "Commande sans suivi disponible: " + order.ae_id);
            continue;
        }

        ShipmentInfo shipment;
        shipment.tracking_number = tracking_number;
        shipment.carrier = logistics_service.empty() ? "AliExpress Standard Shipping" : logistics_service;
        shipment.status = "IN_TRANSIT";
        shipment.last_event = "Numéro de suivi reçu";

        auto tracking = ali_.getTracking(order.ae_id, tracking_number, shipment.carrier);
        if (tracking) shipment = tracking.take();
        else log_.warning("fulfillment", "Suivi détaillé indisponible: " + tracking.error());

        auto fulfillment_orders = shop_.fulfillmentOrders(normalizeShopifyOrderId(order.shopify_id));
        if (!fulfillment_orders) {
            ++counters_.errors;
            log_.warning("fulfillment", "Fulfillment orders Shopify: " + fulfillment_orders.error());
            continue;
        }

        bool submitted = false;
        for (const auto& reference : fulfillment_orders.value()) {
            if (reference.status == "CLOSED" || reference.status == "CANCELLED") continue;
            auto created = shop_.createFulfillment(reference.id, shipment, true);
            if (!created) {
                ++counters_.errors;
                log_.warning("fulfillment", "Création fulfillment Shopify: " + created.error());
                continue;
            }
            submitted = true;
        }
        if (!submitted && !isDelivered(shipment.status)) {
            log_.debug("fulfillment", "Aucun fulfillment order Shopify ouvert pour " + order.shopify_id);
            continue;
        }

        auto saved = db_.saveShipment(order.id, shipment);
        const std::string local_status = isDelivered(shipment.status) ? "delivered" : "shipped";
        auto updated = db_.updateOrderSupplierId(order.shopify_id, order.ae_id, local_status);
        if (!saved || !updated) {
            ++counters_.errors;
            log_.error("fulfillment", !saved ? saved.error() : updated.error());
            continue;
        }
        if (local_status == "delivered") ++counters_.delivered;
        else ++counters_.shipped;
        db_.audit("INFO", "fulfillment", "Suivi synchronisé",
                  "{\"tracking_number\":\"" + shipment.tracking_number + "\"}", order.shopify_id);
        log_.info("fulfillment", "Suivi " + shipment.tracking_number + " synchronisé pour " + order.shopify_id);
    }
}

} // namespace elit21
