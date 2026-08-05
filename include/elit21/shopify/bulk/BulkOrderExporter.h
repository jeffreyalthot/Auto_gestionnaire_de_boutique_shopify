#pragma once
#include "elit21/core/Result.h"
#include "elit21/platform/BusinessComponent.h"
#include "elit21/shopify/ShopifyClient.h"
#include <string>
namespace elit21::shopify::bulk {
class BulkOrderExporter final : public platform::BusinessComponent {
public:
    BulkOrderExporter(); explicit BulkOrderExporter(ShopifyClient& client); void bind(ShopifyClient& client) noexcept {client_=&client;}
    Result<ShopifyBulkOperationRef> start(const std::string& search_filter={});
    platform::OperationResult execute(const platform::OperationContext& context) override;
private: ShopifyClient* client_{nullptr};
};
}
