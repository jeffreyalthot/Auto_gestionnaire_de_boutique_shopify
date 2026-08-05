#include "elit21/config/Config.h"
#include "elit21/json/Json.h"
#include "elit21/net/HttpClient.h"
#include "elit21/shopify/graphql/GraphqlDocument.h"
#include "elit21/shopify/graphql/GraphqlTransport.h"
#include "elit21/shopify/graphql/ShopifyGraphqlClient.h"

#include <fstream>
#include <iostream>
#include <sstream>

int main(int argc, char** argv) {
    const std::string config_path = argc > 1 ? argv[1] : "config/app.json";
    auto config = elit21::Config::load(config_path);
    if (!config) {
        std::cerr << config.error() << '\n';
        return 2;
    }
    std::string query = "query ShopIdentity { shop { id name myshopifyDomain } }";
    if (argc > 2) {
        std::ifstream input(argv[2]);
        if (!input) {
            std::cerr << "Fichier GraphQL introuvable\n";
            return 3;
        }
        std::ostringstream buffer;
        buffer << input.rdbuf();
        query = buffer.str();
    }
    elit21::HttpClient http;
    elit21::shopify::graphql::GraphqlTransport transport(
        http, config.value().shopify.graphqlEndpoint(), config.value().shopify.access_token);
    elit21::shopify::graphql::ShopifyGraphqlClient client(transport);
    elit21::shopify::graphql::GraphqlDocument document("GraphqlQueryTester", query);
    auto response = client.execute(document, elit21::Json::object());
    if (!response) {
        std::cerr << response.error() << '\n';
        return 4;
    }
    std::cout << response.value().dump(true) << '\n';
    return 0;
}
