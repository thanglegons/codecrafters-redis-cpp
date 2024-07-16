#include "Type.hpp"
#include "Parser.h"
#include <optional>

namespace commands {

std::optional<std::string>
Type::inner_handle(const std::span<const std::string> &params,
                   Session *session) {
  int num_params = params.size();
  if (num_params < 1) {
    return std::nullopt;
  }
  const auto &key = params[0];
  auto var_value = data_->get_raw(key);
  if (var_value == std::nullopt) {
    return Parser::encodeString("none");
  }
  if (std::holds_alternative<std::string>(var_value.value())) {
    return Parser::encodeString("string");
  }
  if (std::holds_alternative<Stream>(var_value.value())) {
    return Parser::encodeString("stream");
  }
  return Parser::encodeString("not supported");
}

} // namespace commands