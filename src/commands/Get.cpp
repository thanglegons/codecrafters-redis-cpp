#include "Get.h"
#include "Parser.h"
#include <iterator>
#include <optional>

namespace commands {
Get::Get(const std::vector<std::string>::iterator begin,
         const std::vector<std::string>::iterator end,
         const std::shared_ptr<const KVStorage> data)
    : begin(begin), end(end), data(data) {}

std::optional<std::string> Get::operator()() {
  int num_params = std::distance(begin, end);
  if (num_params < 1) {
    return std::nullopt;
  }
  const auto &key = *begin;
  auto ret = data->get(key);
  if (ret == std::nullopt) {
    return std::nullopt;
  }
  return Parser::encodeString(ret.value());
}

} // namespace commands