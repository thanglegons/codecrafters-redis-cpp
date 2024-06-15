#include "Info.h"
#include "Parser.h"
#include "ReplicationInfo.h"

namespace commands {

Info::Info(const std::vector<std::string>::iterator begin,
           const std::vector<std::string>::iterator end,
           const std::shared_ptr<const ReplicationInfo> replication_info)
    : begin(begin), end(end), replication_info_(std::move(replication_info)) {}

std::optional<std::string> Info::operator()() {
  int num_params = std::distance(begin, end);
  if (num_params != 1) {
    return std::nullopt;
  }
  auto info = replication_info_->returnRawInfo();
  return Parser::encodeBulkString(info);
}
} // namespace commands