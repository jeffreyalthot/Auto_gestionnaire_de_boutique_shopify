#include "elit21/shopify/bulk/BulkOperationMonitor.h"

#include <algorithm>
#include <thread>

namespace elit21::shopify::bulk {

BulkOperationMonitor::BulkOperationMonitor()
    : platform::BusinessComponent("BulkOperationMonitor", "Bounded Shopify bulk operation monitor",
          platform::BusinessComponentSpec{"shopify", "monitor_bulk_operation", {"operation_gid"},
                                          false, true, 1024U * 1024U}) {}

BulkOperationMonitor::BulkOperationMonitor(ShopifyClient& client) : BulkOperationMonitor() { client_ = &client; }

Result<ShopifyBulkOperationRef> BulkOperationMonitor::wait(const std::string& operation_gid,
                                                            std::chrono::seconds timeout,
                                                            std::chrono::milliseconds interval,
                                                            const ProgressCallback& callback) {
    if (!client_) return Result<ShopifyBulkOperationRef>::failure("BulkOperationMonitor is not bound");
    if (operation_gid.empty()) return Result<ShopifyBulkOperationRef>::failure("Bulk operation GID is empty");
    timeout = std::clamp(timeout, std::chrono::seconds(1), std::chrono::seconds(24 * 60 * 60));
    interval = std::clamp(interval, std::chrono::milliseconds(250), std::chrono::milliseconds(30000));
    const auto deadline = std::chrono::steady_clock::now() + timeout;
    while (std::chrono::steady_clock::now() < deadline) {
        auto operation = client_->bulkOperation(operation_gid);
        if (!operation) return operation;
        if (callback) callback(operation.value());
        if (operation.value().terminal()) {
            if (!operation.value().successful()) {
                return Result<ShopifyBulkOperationRef>::failure(
                    "Shopify bulk operation failed: " + operation.value().error_code);
            }
            return operation;
        }
        std::this_thread::sleep_for(interval);
    }
    return Result<ShopifyBulkOperationRef>::failure("Shopify bulk operation timed out");
}

Result<void> BulkOperationMonitor::cancel(const std::string& operation_gid) {
    if (!client_) return Result<void>::failure("BulkOperationMonitor is not bound");
    return client_->cancelBulkOperation(operation_gid);
}

platform::OperationResult BulkOperationMonitor::execute(const platform::OperationContext& context) {
    auto result = executeBusiness(context);
    result.metrics["client_bound"] = client_ ? 1.0 : 0.0;
    result.attributes["polling"] = "bounded";
    return result;
}

} // namespace elit21::shopify::bulk
