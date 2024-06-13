#include "CommandHandler.h"
#include "Parser.h"
#include "Storage.h"
#include <algorithm>

CommandHandler::CommandHandler(std::shared_ptr<KVStorage> data) : data_(data) {}

std::string CommandHandler::handle_raw_command(const std::string &raw_command) {
  auto commands = Parser::parse(raw_command);
  std::string return_message = "+-1\r\n";

  if (commands.empty()) {
    return return_message;
  }

  std::string main_command = commands[0];
  std::transform(main_command.begin(), main_command.end(), main_command.begin(),
                 [](const auto c) { return tolower(c); });

  if (!commands.empty() && main_command == "ping") {
    return_message = "+PONG\r\n";
  } else if (commands.size() == 2 && main_command == "echo") {
    return_message = "+" + commands[1] + "\r\n";
  } else if (commands.size() == 3 && main_command == "set") {
    data_->set(std::move(commands[1]), std::move(commands[2]));
    return_message = "+OK\r\n";
  } else if (commands.size() == 2 && main_command == "get") {
    const auto &key = commands[1];
    auto ret = data_->get(key);
    if (ret == std::nullopt) {
      return_message = "+-1\r\n";
    } else {
      return_message = "+" + ret.value() + "\r\n";
    }
  }

  return return_message;
}