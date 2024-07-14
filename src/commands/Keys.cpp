#include "Keys.hpp"
#include "Parser.h"
#include <optional>

namespace commands {

std::optional<std::string>
Keys::inner_handle(const std::span<const std::string> &params,
                   Session *session) {
  int num_params = params.size();
  if (num_params < 1) {
    return std::nullopt;
  }

  return Parser::encodeRespArray(data_->get_keys());
}

} // namespace commands