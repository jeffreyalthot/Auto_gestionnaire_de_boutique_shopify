#include "elit21/serialization/CsvWriter.h"

namespace elit21::serialization {

CsvWriter::CsvWriter()
    : platform::BusinessComponent(
          "CsvWriter",
          "JSON, CSV and request serialization",
          platform::BusinessComponentSpec{
              "serialization",
              "command",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult CsvWriter::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::serialization
