#include "TestScenarios.h"

bool test_tests_contract_ShopifyGraphqlContractTests() {
    return elit21::tests::graphqlScenario() && elit21::tests::shopifyAutonomyScenario() && elit21::tests::shopifyInfrastructureScenario();
}
