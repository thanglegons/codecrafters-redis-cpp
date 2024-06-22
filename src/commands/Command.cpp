#include "Command.h"
#include "Session.h"
#include <iostream>

namespace commands {
void Command::handle() {
  std::optional<std::string> return_message = inner_handle();
  if (return_message.has_value()) {
    session_->write(return_message.value(), default_call_back);
  } else {
    session_->write(kErrorReturn, default_call_back);
  }
}
void Command::default_call_back(const asio::error_code &error_code, size_t len) {
  if (!error_code) {
    std::cout << "error = " << error_code.message() << "\n";
  }
}
} // namespace commands