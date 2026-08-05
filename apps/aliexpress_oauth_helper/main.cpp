#include "elit21/aliexpress/auth/AliExpressAuthorizationUrlBuilder.h"
#include "elit21/aliexpress/auth/AliExpressOAuthClient.h"
#include "elit21/config/Config.h"
#include "elit21/net/HttpClient.h"
#include "elit21/security/Crypto.h"

#include <iostream>
#include <string>

int main(int argc, char** argv) {
    const std::string config_path = argc > 1 ? argv[1] : "config/app.json";
    auto config = elit21::Config::load(config_path);
    if (!config) {
        std::cerr << "Configuration invalide: " << config.error() << '\n';
        return 2;
    }
    const std::string redirect_uri = argc > 2 ? argv[2] : "https://elit21.com/aliexpress/callback";
    const std::string state = elit21::crypto::randomHex(24);
    std::cout << "ELIT21 AliExpress OAuth helper\n"
              << elit21::aliexpress::auth::AliExpressAuthorizationUrlBuilder::build(
                     config.value().aliexpress.app_key, redirect_uri, state, true)
              << "\nstate=" << state << '\n';

    if (argc >= 4) {
        elit21::HttpClient http;
        elit21::aliexpress::auth::AliExpressOAuthClient oauth(http);
        auto token = oauth.createToken(config.value().aliexpress.app_key,
                                       config.value().aliexpress.app_secret,
                                       argv[3]);
        if (!token) {
            std::cerr << "Échange du code impossible: " << token.error() << '\n';
            return 3;
        }
        std::cout << "access_token_recu=true\nexpires_in=" << token.value().expires_in_seconds
                  << "\nrefresh_token_recu=" << (!token.value().refresh_token.empty()) << '\n';
    }
    return 0;
}
