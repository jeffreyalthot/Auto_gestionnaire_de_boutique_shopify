#pragma once

#include "elit21/core/Result.h"
#include "elit21/net/HttpClient.h"
#include "elit21/platform/BusinessComponent.h"

#include <cstddef>
#include <string>

namespace elit21::shopify::bulk {

struct BulkDownloadResult {
    std::string path;
    std::size_t bytes{0};
    std::string sha256;
};

class BulkDownloadManager final : public platform::BusinessComponent {
public:
    BulkDownloadManager();
    explicit BulkDownloadManager(HttpClient& http);
    void bind(HttpClient& http) noexcept { http_ = &http; }
    [[nodiscard]] bool bound() const noexcept { return http_ != nullptr; }
    Result<BulkDownloadResult> download(const std::string& url, const std::string& destination,
                                        std::size_t maximum_bytes = 512U * 1024U * 1024U);
    platform::OperationResult execute(const platform::OperationContext& context) override;
private:
    HttpClient* http_{nullptr};
};

} // namespace elit21::shopify::bulk
