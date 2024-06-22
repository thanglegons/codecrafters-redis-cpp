#include "Info.h"
#include "Parser.h"

namespace commands {

std::optional<std::string> Info::inner_handle() {
  int num_params = params_.size();
  if (num_params != 1) {
    return std::nullopt;
  }
  auto info = replication_info_->returnRawInfo();
  return Parser::encodeBulkString(info);
}
} // namespace commands