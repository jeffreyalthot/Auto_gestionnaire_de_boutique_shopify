#include "TestScenarios.h"

bool test_tests_security_SqlInjectionResistanceTests() {
    return elit21::tests::databaseScenario();
}
