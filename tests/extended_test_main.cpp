#include "elit21/pricing/PricingEngine.h"
#include "elit21/config/Config.h"
#include "elit21/platform/ManagedComponent.h"
#include <cmath>
#include <iostream>

bool test_tests_contract_AliExpressIopContractTests();
bool test_tests_contract_AliExpressTopContractTests();
bool test_tests_contract_MappingContractTests();
bool test_tests_contract_ShopifyGraphqlContractTests();
bool test_tests_contract_ShopifyWebhookContractTests();
bool test_tests_end_to_end_CustomerOrderEndToEndTests();
bool test_tests_end_to_end_FulfillmentEndToEndTests();
bool test_tests_end_to_end_InventorySyncEndToEndTests();
bool test_tests_end_to_end_ProductImportEndToEndTests();
bool test_tests_end_to_end_RecoveryEndToEndTests();
bool test_tests_end_to_end_RefundEndToEndTests();
bool test_tests_end_to_end_SupplierOrderEndToEndTests();
bool test_tests_fault_injection_AliExpressTimeoutTests();
bool test_tests_fault_injection_DatabaseFailureTests();
bool test_tests_fault_injection_DuplicateWebhookTests();
bool test_tests_fault_injection_NetworkFailureTests();
bool test_tests_fault_injection_ProcessCrashRecoveryTests();
bool test_tests_fault_injection_ShopifyThrottleTests();
bool test_tests_fault_injection_TokenExpirationTests();
bool test_tests_integration_AliExpressFreightIntegrationTests();
bool test_tests_integration_AliExpressOrderIntegrationTests();
bool test_tests_integration_AliExpressProductIntegrationTests();
bool test_tests_integration_AliExpressTrackingIntegrationTests();
bool test_tests_integration_AliExpressTransportIntegrationTests();
bool test_tests_integration_DatabaseIntegrationTests();
bool test_tests_integration_ShopifyBulkOperationIntegrationTests();
bool test_tests_integration_ShopifyGraphqlIntegrationTests();
bool test_tests_integration_ShopifyWebhookIntegrationTests();
bool test_tests_integration_WorkflowIntegrationTests();
bool test_tests_performance_CatalogSyncBenchmark();
bool test_tests_performance_DatabaseBenchmark();
bool test_tests_performance_InventorySyncBenchmark();
bool test_tests_performance_OrderProcessingBenchmark();
bool test_tests_performance_WebhookThroughputBenchmark();
bool test_tests_security_CredentialEncryptionTests();
bool test_tests_security_HmacVerificationTests();
bool test_tests_security_ReplayProtectionTests();
bool test_tests_security_RequestSigningTests();
bool test_tests_security_SecretRedactionTests();
bool test_tests_security_SqlInjectionResistanceTests();
bool test_tests_unit_aliexpress_AliexpressModuleTests();
bool test_tests_unit_automation_AutomationModuleTests();
bool test_tests_unit_catalog_CatalogModuleTests();
bool test_tests_unit_compliance_ComplianceModuleTests();
bool test_tests_unit_configuration_ConfigurationModuleTests();
bool test_tests_unit_core_CoreModuleTests();
bool test_tests_unit_finance_FinanceModuleTests();
bool test_tests_unit_fulfillment_FulfillmentModuleTests();
bool test_tests_unit_inventory_InventoryModuleTests();
bool test_tests_unit_mapping_MappingModuleTests();
bool test_tests_unit_orders_OrdersModuleTests();
bool test_tests_unit_pricing_PricingModuleTests();
bool test_tests_unit_returns_ReturnsModuleTests();
bool test_tests_unit_risk_RiskModuleTests();
bool test_tests_unit_security_SecurityModuleTests();
bool test_tests_unit_shopify_ShopifyModuleTests();
bool test_tests_unit_sourcing_SourcingModuleTests();
bool test_tests_unit_storage_StorageModuleTests();
bool test_tests_unit_terminal_TerminalModuleTests();

int main() {
    int failures = 0;
    const auto check = [&failures](bool value, const char* name) {
        if (!value) { std::cerr << "FAIL: " << name << "\n"; ++failures; }
        else { std::cout << "PASS: " << name << "\n"; }
    };
    elit21::PricingConfig pricing_config;
    pricing_config.markup_percent_before_shipping = 100.0;
    elit21::PricingEngine pricing(pricing_config);
    const auto quote = pricing.calculate(10.0, 3.0);
    check(static_cast<bool>(quote), "pricing calculation");
    if (quote) {
        check(std::fabs(quote.value().price_before_shipping_cad - 20.0) < 0.001, "100 percent markup before shipping");
        check(std::fabs(quote.value().final_price_cad - 23.0) < 0.001, "shipping added after markup");
    }
    check(test_tests_contract_AliExpressIopContractTests(), "test_tests_contract_AliExpressIopContractTests");
    check(test_tests_contract_AliExpressTopContractTests(), "test_tests_contract_AliExpressTopContractTests");
    check(test_tests_contract_MappingContractTests(), "test_tests_contract_MappingContractTests");
    check(test_tests_contract_ShopifyGraphqlContractTests(), "test_tests_contract_ShopifyGraphqlContractTests");
    check(test_tests_contract_ShopifyWebhookContractTests(), "test_tests_contract_ShopifyWebhookContractTests");
    check(test_tests_end_to_end_CustomerOrderEndToEndTests(), "test_tests_end_to_end_CustomerOrderEndToEndTests");
    check(test_tests_end_to_end_FulfillmentEndToEndTests(), "test_tests_end_to_end_FulfillmentEndToEndTests");
    check(test_tests_end_to_end_InventorySyncEndToEndTests(), "test_tests_end_to_end_InventorySyncEndToEndTests");
    check(test_tests_end_to_end_ProductImportEndToEndTests(), "test_tests_end_to_end_ProductImportEndToEndTests");
    check(test_tests_end_to_end_RecoveryEndToEndTests(), "test_tests_end_to_end_RecoveryEndToEndTests");
    check(test_tests_end_to_end_RefundEndToEndTests(), "test_tests_end_to_end_RefundEndToEndTests");
    check(test_tests_end_to_end_SupplierOrderEndToEndTests(), "test_tests_end_to_end_SupplierOrderEndToEndTests");
    check(test_tests_fault_injection_AliExpressTimeoutTests(), "test_tests_fault_injection_AliExpressTimeoutTests");
    check(test_tests_fault_injection_DatabaseFailureTests(), "test_tests_fault_injection_DatabaseFailureTests");
    check(test_tests_fault_injection_DuplicateWebhookTests(), "test_tests_fault_injection_DuplicateWebhookTests");
    check(test_tests_fault_injection_NetworkFailureTests(), "test_tests_fault_injection_NetworkFailureTests");
    check(test_tests_fault_injection_ProcessCrashRecoveryTests(), "test_tests_fault_injection_ProcessCrashRecoveryTests");
    check(test_tests_fault_injection_ShopifyThrottleTests(), "test_tests_fault_injection_ShopifyThrottleTests");
    check(test_tests_fault_injection_TokenExpirationTests(), "test_tests_fault_injection_TokenExpirationTests");
    check(test_tests_integration_AliExpressFreightIntegrationTests(), "test_tests_integration_AliExpressFreightIntegrationTests");
    check(test_tests_integration_AliExpressOrderIntegrationTests(), "test_tests_integration_AliExpressOrderIntegrationTests");
    check(test_tests_integration_AliExpressProductIntegrationTests(), "test_tests_integration_AliExpressProductIntegrationTests");
    check(test_tests_integration_AliExpressTrackingIntegrationTests(), "test_tests_integration_AliExpressTrackingIntegrationTests");
    check(test_tests_integration_AliExpressTransportIntegrationTests(), "test_tests_integration_AliExpressTransportIntegrationTests");
    check(test_tests_integration_DatabaseIntegrationTests(), "test_tests_integration_DatabaseIntegrationTests");
    check(test_tests_integration_ShopifyBulkOperationIntegrationTests(), "test_tests_integration_ShopifyBulkOperationIntegrationTests");
    check(test_tests_integration_ShopifyGraphqlIntegrationTests(), "test_tests_integration_ShopifyGraphqlIntegrationTests");
    check(test_tests_integration_ShopifyWebhookIntegrationTests(), "test_tests_integration_ShopifyWebhookIntegrationTests");
    check(test_tests_integration_WorkflowIntegrationTests(), "test_tests_integration_WorkflowIntegrationTests");
    check(test_tests_performance_CatalogSyncBenchmark(), "test_tests_performance_CatalogSyncBenchmark");
    check(test_tests_performance_DatabaseBenchmark(), "test_tests_performance_DatabaseBenchmark");
    check(test_tests_performance_InventorySyncBenchmark(), "test_tests_performance_InventorySyncBenchmark");
    check(test_tests_performance_OrderProcessingBenchmark(), "test_tests_performance_OrderProcessingBenchmark");
    check(test_tests_performance_WebhookThroughputBenchmark(), "test_tests_performance_WebhookThroughputBenchmark");
    check(test_tests_security_CredentialEncryptionTests(), "test_tests_security_CredentialEncryptionTests");
    check(test_tests_security_HmacVerificationTests(), "test_tests_security_HmacVerificationTests");
    check(test_tests_security_ReplayProtectionTests(), "test_tests_security_ReplayProtectionTests");
    check(test_tests_security_RequestSigningTests(), "test_tests_security_RequestSigningTests");
    check(test_tests_security_SecretRedactionTests(), "test_tests_security_SecretRedactionTests");
    check(test_tests_security_SqlInjectionResistanceTests(), "test_tests_security_SqlInjectionResistanceTests");
    check(test_tests_unit_aliexpress_AliexpressModuleTests(), "test_tests_unit_aliexpress_AliexpressModuleTests");
    check(test_tests_unit_automation_AutomationModuleTests(), "test_tests_unit_automation_AutomationModuleTests");
    check(test_tests_unit_catalog_CatalogModuleTests(), "test_tests_unit_catalog_CatalogModuleTests");
    check(test_tests_unit_compliance_ComplianceModuleTests(), "test_tests_unit_compliance_ComplianceModuleTests");
    check(test_tests_unit_configuration_ConfigurationModuleTests(), "test_tests_unit_configuration_ConfigurationModuleTests");
    check(test_tests_unit_core_CoreModuleTests(), "test_tests_unit_core_CoreModuleTests");
    check(test_tests_unit_finance_FinanceModuleTests(), "test_tests_unit_finance_FinanceModuleTests");
    check(test_tests_unit_fulfillment_FulfillmentModuleTests(), "test_tests_unit_fulfillment_FulfillmentModuleTests");
    check(test_tests_unit_inventory_InventoryModuleTests(), "test_tests_unit_inventory_InventoryModuleTests");
    check(test_tests_unit_mapping_MappingModuleTests(), "test_tests_unit_mapping_MappingModuleTests");
    check(test_tests_unit_orders_OrdersModuleTests(), "test_tests_unit_orders_OrdersModuleTests");
    check(test_tests_unit_pricing_PricingModuleTests(), "test_tests_unit_pricing_PricingModuleTests");
    check(test_tests_unit_returns_ReturnsModuleTests(), "test_tests_unit_returns_ReturnsModuleTests");
    check(test_tests_unit_risk_RiskModuleTests(), "test_tests_unit_risk_RiskModuleTests");
    check(test_tests_unit_security_SecurityModuleTests(), "test_tests_unit_security_SecurityModuleTests");
    check(test_tests_unit_shopify_ShopifyModuleTests(), "test_tests_unit_shopify_ShopifyModuleTests");
    check(test_tests_unit_sourcing_SourcingModuleTests(), "test_tests_unit_sourcing_SourcingModuleTests");
    check(test_tests_unit_storage_StorageModuleTests(), "test_tests_unit_storage_StorageModuleTests");
    check(test_tests_unit_terminal_TerminalModuleTests(), "test_tests_unit_terminal_TerminalModuleTests");
    std::cout << "Extended tests: " << (failures == 0 ? "SUCCESS" : "FAILED") << "\n";
    return failures == 0 ? 0 : 1;
}
