#include "elit21/serialization/CsvReader.h"

namespace elit21::serialization {

CsvReader::CsvReader()
    : platform::BusinessComponent(
          "CsvReader",
          "JSON, CSV and request serialization",
          platform::BusinessComponentSpec{
              "serialization",
              "query",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult CsvReader::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::serialization
