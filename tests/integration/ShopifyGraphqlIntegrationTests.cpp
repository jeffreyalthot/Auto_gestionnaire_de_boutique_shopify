#include "TestScenarios.h"

bool test_tests_integration_ShopifyGraphqlIntegrationTests() {
    return elit21::tests::graphqlScenario() && elit21::tests::shopifyAutonomyScenario() && elit21::tests::shopifyInfrastructureScenario() && elit21::tests::shopifyGovernanceScenario();
}
