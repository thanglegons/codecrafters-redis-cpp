#include "Info.h"
#include "Parser.h"

namespace commands {

Info::Info(const std::vector<std::string>::iterator begin,
           const std::vector<std::string>::iterator end)
    : begin(begin), end(end) {}

std::optional<std::string> Info::operator()() {
  int num_params = std::distance(begin, end);
  if (num_params != 1) {
    return std::nullopt;
  }
  return Parser::encodeBulkString("role:master");
}
} // namespace commands