#include "CommandHandler.h"
#include "Parser.h"
#include "Storage.h"
#include "commands/Echo.h"
#include "commands/Get.h"
#include "commands/Info.h"
#include "commands/Ping.h"
#include "commands/Set.h"
#include <algorithm>

CommandHandler::CommandHandler(std::shared_ptr<KVStorage> data) : data_(data) {}

std::string CommandHandler::handle_raw_command(const std::string &raw_command) {
  auto commands = Parser::decode(raw_command);

  if (commands.empty()) {
    return kErrorReturn;
  }

  std::string main_command = commands[0];
  std::transform(main_command.begin(), main_command.end(), main_command.begin(),
                 [](const auto c) { return tolower(c); });

  std::optional<std::string> opt_return_message;
  if (main_command == "ping") {
    opt_return_message = commands::Ping()();
  }
  if (main_command == "echo") {
    opt_return_message = commands::Echo(commands.begin() + 1, commands.end())();
  } else if (main_command == "set") {
    opt_return_message =
        commands::Set(commands.begin() + 1, commands.end(), data_)();
  } else if (main_command == "get") {
    opt_return_message =
        commands::Get(commands.begin() + 1, commands.end(), data_)();
  } else if (main_command == "info") {
    opt_return_message = commands::Info(commands.begin() + 1, commands.end())();
  }

  if (!opt_return_message.has_value()) {
    return kErrorReturn;
  }
  return opt_return_message.value();
}