#include "CommandHandler.h"
#include "Parser.h"
#include "Storage.h"
#include "commands/Echo.h"
#include "commands/Get.h"
#include "commands/Info.h"
#include "commands/Ping.h"
#include "commands/Replconf.h"
#include "commands/Set.h"
#include <algorithm>

CommandHandler::CommandHandler(
    std::shared_ptr<KVStorage> data,
    std::shared_ptr<ReplicationInfo> replication_info)
    : data_(std::move(data)), replication_info_(std::move(replication_info)) {}

std::string CommandHandler::handle_raw_command(const std::string &raw_command) {
  auto commands = Parser::decode(raw_command);

  if (commands.empty()) {
    return kErrorReturn;
  }

  std::string main_command = commands[0];
  std::transform(main_command.begin(), main_command.end(), main_command.begin(),
                 [](const auto c) { return tolower(c); });
  
  auto opt_return_message = [&]() -> std::optional<std::string> {
    if (main_command == "ping") {
      return commands::Ping()();
    } else if (main_command == "echo") {
      return commands::Echo(commands.begin() + 1, commands.end())();
    } else if (main_command == "set") {
      return commands::Set(commands.begin() + 1, commands.end(), data_)();
    } else if (main_command == "get") {
      return commands::Get(commands.begin() + 1, commands.end(), data_)();
    } else if (main_command == "info") {
      return commands::Info(commands.begin() + 1, commands.end(),
                            replication_info_)();
    } else if (main_command == "replconf") {
      return commands::Replconf(commands.begin() + 1, commands.end())();
    }
    return std::nullopt;
  }();

  if (!opt_return_message.has_value()) {
    return kErrorReturn;
  }
  return opt_return_message.value();
}