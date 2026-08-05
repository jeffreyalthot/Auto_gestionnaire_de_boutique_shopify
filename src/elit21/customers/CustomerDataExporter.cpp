#include "elit21/customers/CustomerDataExporter.h"

namespace elit21::customers {

CustomerDataExporter::CustomerDataExporter()
    : platform::BusinessComponent(
          "CustomerDataExporter",
          "customer data lifecycle management",
          platform::BusinessComponentSpec{
              "customers",
              "command",
              std::vector<std::string>{"customer_id"},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult CustomerDataExporter::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::customers
