#include "elit21/terminal/CommandParser.h"

namespace elit21::terminal {

CommandParser::CommandParser()
    : platform::BusinessComponent(
          "CommandParser",
          "fixed-position terminal dashboard and command handling",
          platform::BusinessComponentSpec{
              "terminal",
              "validate_transform",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult CommandParser::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::terminal
