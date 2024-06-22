#include "Get.h"
#include "Parser.h"
#include <optional>

namespace commands {

std::optional<std::string> Get::inner_handle() {
  int num_params = params_.size();
  if (num_params < 1) {
    return std::nullopt;
  }
  const auto &key = params_[0];
  auto ret = data_->get(key);
  if (ret == std::nullopt) {
    return std::nullopt;
  }
  return Parser::encodeString(ret.value());
}

} // namespace commands