#include "Echo.h"
#include "Parser.h"

namespace commands {

Echo::Echo(const std::vector<std::string>::iterator begin,
           const std::vector<std::string>::iterator end)
    : begin(begin), end(end) {}

std::optional<std::string> Echo::operator()() {
  int num_params = std::distance(begin, end);
  if (num_params != 1) {
    return std::nullopt;
  }
  return Parser::encodeString(*begin);
}
} // namespace commands