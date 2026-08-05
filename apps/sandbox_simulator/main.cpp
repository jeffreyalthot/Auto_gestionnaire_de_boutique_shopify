#include "elit21/config/Config.h"
#include "elit21/pricing/PricingEngine.h"

#include <charconv>
#include <cmath>
#include <iostream>
#include <string>

namespace {
bool parseAmount(const char* text, double& value) {
    if (!text) return false;
    const std::string input(text);
    const auto* begin = input.data();
    const auto* end = begin + input.size();
    const auto result = std::from_chars(begin, end, value);
    return result.ec == std::errc{} && result.ptr == end && std::isfinite(value) && value >= 0.0;
}
}

int main(int argc, char** argv) {
    if (argc > 1 && (std::string(argv[1]) == "--help" || std::string(argv[1]) == "-h")) {
        std::cout << "Usage: elit21_sandbox_simulator [supplier_cost_cad] [shipping_cad] [markup_percent]\n";
        return 0;
    }
    double cost = 10.0;
    double shipping = 3.0;
    double markup = 100.0;
    if ((argc > 1 && !parseAmount(argv[1], cost)) ||
        (argc > 2 && !parseAmount(argv[2], shipping)) ||
        (argc > 3 && !parseAmount(argv[3], markup))) {
        std::cerr << "Montant invalide. Utilisez des nombres positifs, par exemple: 10 3 100\n";
        return 2;
    }
    if (markup < 100.0) {
        std::cerr << "La majoration minimale du projet est de 100 % avant livraison\n";
        return 3;
    }
    elit21::PricingConfig configuration;
    configuration.markup_percent_before_shipping = markup;
    elit21::PricingEngine pricing(configuration);
    auto quote = pricing.calculate(cost, shipping);
    if (!quote) {
        std::cerr << quote.error() << '\n';
        return 4;
    }
    std::cout << "supplier_cost=" << quote.value().supplier_cost_cad << '\n'
              << "markup_percent=" << markup << '\n'
              << "markup=" << quote.value().markup_amount_cad << '\n'
              << "price_before_shipping=" << quote.value().price_before_shipping_cad << '\n'
              << "shipping=" << quote.value().shipping_cad << '\n'
              << "final_price=" << quote.value().final_price_cad << '\n';
    return 0;
}
