#include "elit21/shopify/bulk/BulkDownloadManager.h"

#include "elit21/security/Crypto.h"

#include <filesystem>
#include <fstream>

namespace elit21::shopify::bulk {

BulkDownloadManager::BulkDownloadManager()
    : platform::BusinessComponent("BulkDownloadManager", "Verified Shopify bulk result downloader",
          platform::BusinessComponentSpec{"shopify", "download_bulk_result", {"url", "destination"},
                                          true, false, 8U * 1024U * 1024U}) {}

BulkDownloadManager::BulkDownloadManager(HttpClient& http) : BulkDownloadManager() { http_ = &http; }

Result<BulkDownloadResult> BulkDownloadManager::download(const std::string& url,
                                                          const std::string& destination,
                                                          std::size_t maximum_bytes) {
    if (!http_) return Result<BulkDownloadResult>::failure("BulkDownloadManager is not bound to HttpClient");
    if (url.rfind("https://", 0) != 0) return Result<BulkDownloadResult>::failure("Bulk download URL must use HTTPS");
    if (destination.empty()) return Result<BulkDownloadResult>::failure("Bulk destination path is empty");
    http_->setMaximumResponseBytes(maximum_bytes);
    auto response = http_->get(url, {{"Accept", "application/jsonl,application/x-ndjson,text/plain"}});
    if (!response) return Result<BulkDownloadResult>::failure(response.error());
    if (response.value().status < 200 || response.value().status >= 300) {
        return Result<BulkDownloadResult>::failure("Bulk download HTTP " + std::to_string(response.value().status));
    }
    if (response.value().body.size() > maximum_bytes) {
        return Result<BulkDownloadResult>::failure("Bulk result exceeds maximum allowed size");
    }
    const std::filesystem::path final_path(destination);
    const auto temporary = final_path.string() + ".part";
    std::error_code error;
    if (final_path.has_parent_path()) std::filesystem::create_directories(final_path.parent_path(), error);
    std::ofstream output(temporary, std::ios::binary | std::ios::trunc);
    if (!output) return Result<BulkDownloadResult>::failure("Cannot create bulk destination file");
    output.write(response.value().body.data(), static_cast<std::streamsize>(response.value().body.size()));
    output.close();
    if (!output) return Result<BulkDownloadResult>::failure("Failed while writing bulk destination file");
    std::filesystem::rename(temporary, final_path, error);
    if (error) {
        std::filesystem::remove(final_path, error);
        error.clear();
        std::filesystem::rename(temporary, final_path, error);
    }
    if (error) return Result<BulkDownloadResult>::failure("Cannot finalize bulk destination file: " + error.message());
    return Result<BulkDownloadResult>::success(
        {final_path.string(), response.value().body.size(), crypto::sha256Hex(response.value().body)});
}

platform::OperationResult BulkDownloadManager::execute(const platform::OperationContext& context) {
    auto base = executeBusiness(context);
    base.metrics["http_bound"] = bound() ? 1.0 : 0.0;
    base.attributes["atomic_write"] = "true";
    base.attributes["tls_required"] = "true";
    return base;
}

} // namespace elit21::shopify::bulk
