#include "Command.h"
#include "Parser.h"
#include "Session.h"
#include <iostream>

namespace commands {
void Command::handle(std::string raw_command,
                     Session* session) {
  auto commands = Parser::decode(raw_command);
  std::string main_command = commands[0];
  std::transform(main_command.begin(), main_command.end(), main_command.begin(),
                 [](const auto c) { return tolower(c); });
  std::span<std::string> params{commands.begin() + 1, commands.end()};

  std::optional<std::string> return_message = inner_handle(params, session);

  if (!session->is_master_session()) {
    // don't write back if it's from master
    if (return_message.has_value()) {
      session->write(return_message.value(), default_call_back);
    } else {
      session->write(kErrorReturn, default_call_back);
    }
  }

  after_write(params, session);

  if (is_write_command) {
    const auto replicas = session->get_replicas();
    for (auto &replica : replicas) {
      if (replica->is_session_closed()) {
        continue;
      }
      replica->write(
          raw_command, [](const asio::error_code &error_code, size_t len) {
            if (error_code) {
              std::cout << "Failed to propagate to replicas, error = "
                        << error_code.message() << std::endl;
            }
          });
    }
  }
}
void Command::default_call_back(const asio::error_code &error_code,
                                size_t len) {
  if (error_code) {
    std::cout << "error = " << error_code.message() << "\n";
  }
}
} // namespace commands