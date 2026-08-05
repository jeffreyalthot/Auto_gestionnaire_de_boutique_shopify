#include "elit21/catalog/ImageImportPipeline.h"

namespace elit21::catalog {

ImageImportPipeline::ImageImportPipeline()
    : platform::BusinessComponent(
          "ImageImportPipeline",
          "catalog normalization and publication",
          platform::BusinessComponentSpec{
              "catalog",
              "command",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ImageImportPipeline::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::catalog
