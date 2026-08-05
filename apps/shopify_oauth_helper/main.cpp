#include "elit21/config/Config.h"
#include "elit21/net/HttpClient.h"
#include "elit21/security/Crypto.h"
#include "elit21/shopify/ShopifyScopes.h"
#include "elit21/shopify/auth/ShopifyAuthorizationUrlBuilder.h"
#include "elit21/shopify/auth/ShopifyOAuthClient.h"

#include <iostream>
#include <string>

int main(int argc, char** argv) {
    const std::string config_path = argc > 1 ? argv[1] : "config/app.json";
    auto config = elit21::Config::load(config_path);
    if (!config) {
        std::cerr << "Configuration invalide: " << config.error() << '\n';
        return 2;
    }
    const std::string client_id = argc > 2 ? argv[2] : "SHOPIFY_CLIENT_ID";
    const std::string redirect_uri = argc > 3 ? argv[3] : "https://elit21.com/shopify/callback";
    const std::string state = elit21::crypto::randomHex(24);
    const auto scopes = elit21::shopify::ShopifyScopes::autonomousManagerDefaults();
    std::cout << "ELIT21 Shopify OAuth helper\n"
              << elit21::shopify::auth::ShopifyAuthorizationUrlBuilder::build(
                     config.value().shopify.shop, client_id, scopes, redirect_uri, state)
              << "\nstate=" << state << '\n';

    if (argc >= 6) {
        elit21::HttpClient http;
        elit21::shopify::auth::ShopifyOAuthClient oauth(http);
        auto token = oauth.exchangeCode(config.value().shopify.shop, client_id, argv[4], argv[5]);
        if (!token) {
            std::cerr << "Échange du code impossible: " << token.error() << '\n';
            return 3;
        }
        std::cout << "access_token_recu=true\nscopes=" << token.value().scope << '\n';
    }
    return 0;
}
