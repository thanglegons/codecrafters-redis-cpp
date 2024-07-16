#include "XADD.hpp"
#include "Parser.h"

namespace commands {

std::optional<std::string>
XADD::inner_handle(const std::span<const std::string> &params,
                   Session *session) {
  int num_params = params.size();
  if (num_params < 3) {
    return std::nullopt;
  }
  auto key = params[0];
  auto id = params[1];
  std::vector<std::pair<std::string, std::string>> pairs;
  for (int i = 2; i < params.size(); i += 2) {
    pairs.emplace_back(params[i], params[i + 1]);
  }
  auto ret = Parser::encodeString(id);
  data_->set_stream(std::move(key), std::move(id), std::move(pairs));
  return ret;
}

} // namespace commands