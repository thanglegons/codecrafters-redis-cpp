#include "Type.hpp"
#include "Parser.h"
#include <optional>

namespace commands {

std::optional<std::string> Type::inner_handle(const std::span<const std::string>& params, Session* session) {
  int num_params = params.size();
  if (num_params < 1) {
    return std::nullopt;
  }
  const auto &key = params[0];
  auto ret = data_->get(key);
  if (ret == std::nullopt) {
    return Parser::encodeString("none");
  }
  return Parser::encodeString("string");
}

} // namespace commands