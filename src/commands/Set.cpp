#include "Set.h"
#include <iterator>
#include "Parser.h"

namespace commands {
Set::Set(const std::vector<std::string>::iterator begin,
         const std::vector<std::string>::iterator end,
         const std::shared_ptr<KVStorage> data)
    : begin(begin), end(end), data(data) {}

std::optional<std::string> Set::operator()() {
  int num_params = std::distance(begin, end);
  if (num_params < 2) {
    return std::nullopt;
  }
  auto key = *begin;
  auto value = *(begin + 1);
  if (num_params == 2) {
    data->set(std::move(key), std::move(value));
    return Parser::encodeString("OK");
  } else if (num_params == 4 && *(begin + 2) == "px") {
    auto expiring_time = std::stoi(*(begin + 3));
    data->set(std::move(key), std::move(value), expiring_time);
    return Parser::encodeString("OK");
  }
  return std::nullopt;
}

} // namespace commands