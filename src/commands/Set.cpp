#include "Set.h"
#include "Parser.h"

namespace commands {

std::optional<std::string> Set::inner_handle() {
  int num_params = params_.size();
  if (num_params < 2) {
    return std::nullopt;
  }
  auto key = params_[0];
  auto value = params_[1];
  if (num_params == 2) {
    data_->set(std::move(key), std::move(value));
    return Parser::encodeString("OK");
  } else if (num_params == 4 && params_[2] == "px") {
    auto expiring_time = std::stoi(params_[3]);
    data_->set(std::move(key), std::move(value), expiring_time);
    return Parser::encodeString("OK");
  }
  return std::nullopt;
}

} // namespace commands