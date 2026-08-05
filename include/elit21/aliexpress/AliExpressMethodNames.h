#pragma once
#include <set>
#include <string>
namespace elit21::aliexpress {
class AliExpressMethodNames{public:static const std::string&productGet();static const std::string&recommendFeed();static const std::string&imageSearch();static const std::string&freightCalculate();static const std::string&placeOrder();static const std::string&orderGet();static const std::string&trackingQuery();static const std::string&addDropshippingInfo();static const std::string&commissionOrders();static const std::string&submitOrderData();static bool supported(const std::string&method);static const std::set<std::string>&all();};
}
