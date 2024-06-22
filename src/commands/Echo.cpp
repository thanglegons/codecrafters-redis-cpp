#include "Echo.h"
#include "Parser.h"

namespace commands {

std::optional<std::string> Echo::inner_handle() {
  int num_params = params_.size();
  if (num_params != 1) {
    return std::nullopt;
  }
  auto parsed_message = Parser::encodeString(params_[0]);
  return parsed_message;
}

} // namespace commands