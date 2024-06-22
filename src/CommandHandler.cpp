#include "CommandHandler.h"
#include "Parser.h"
#include "Storage.h"
#include "commands/Echo.h"
#include "commands/Get.h"
#include "commands/Info.h"
#include "commands/Ping.h"
#include "commands/Psync.h"
#include "commands/Replconf.h"
#include "commands/Set.h"
#include <algorithm>

CommandHandler::CommandHandler(
    std::shared_ptr<KVStorage> data,
    std::shared_ptr<ReplicationInfo> replication_info, Session *session)
    : data_(std::move(data)), replication_info_(std::move(replication_info)),
      session_(session) {}

void CommandHandler::handle_raw_command(const std::string &raw_command) {
  auto commands = Parser::decode(raw_command);

  std::string main_command = commands[0];
  std::transform(main_command.begin(), main_command.end(), main_command.begin(),
                 [](const auto c) { return tolower(c); });

  std::span<std::string> params{commands.begin() + 1, commands.end()};

  if (main_command == "ping") {
    commands::Ping(params, session_).handle();
  } else if (main_command == "echo") {
    commands::Echo(params, session_).handle();
  } else if (main_command == "set") {
    commands::Set(params, session_, data_).handle();
  } else if (main_command == "get") {
    commands::Get(params, session_, data_).handle();
  } else if (main_command == "info") {
    commands::Info(params, session_, replication_info_).handle();
  } else if (main_command == "replconf") {
    commands::Replconf(params, session_).handle();
  } else if (main_command == "psync") {
    commands::Psync(params, session_, replication_info_).handle();
  }
}