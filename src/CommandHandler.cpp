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
#include "Session.h"
#include <algorithm>
#include <iostream>
#include <memory>
#include "ReplicationInfo.h"

CommandHandler::CommandHandler(
    std::shared_ptr<KVStorage> data,
    std::shared_ptr<ReplicationInfo> replication_info, Session *session)
    : data_(std::move(data)), replication_info_(std::move(replication_info)),
      session_(session) {}

void CommandHandler::handle_raw_command(const std::string &raw_command)
{
  auto commands = Parser::decode(raw_command);
  for (const auto &command_list : commands)
  {
    std::string main_command = command_list[0];
    std::transform(main_command.begin(), main_command.end(),
                   main_command.begin(),
                   [](const auto c)
                   { return tolower(c); });

    auto command = [&]() -> std::unique_ptr<commands::Command>
    {
      if (main_command == "ping")
      {
        return std::make_unique<commands::Ping>();
      }
      else if (main_command == "echo")
      {
        return std::make_unique<commands::Echo>();
      }
      else if (main_command == "set")
      {
        return std::make_unique<commands::Set>(data_);
      }
      else if (main_command == "get")
      {
        return std::make_unique<commands::Get>(data_);
      }
      else if (main_command == "info")
      {
        return std::make_unique<commands::Info>(replication_info_);
      }
      else if (main_command == "replconf")
      {
        return std::make_unique<commands::Replconf>(replication_info_);
      }
      else if (main_command == "psync")
      {
        return std::make_unique<commands::Psync>(replication_info_);
      }
      return nullptr;
    }();

    if (command == nullptr)
    {
      std::cout << "Incorrect command, command = " << main_command << "\n";
      return;
    }

    auto callback = [this, offset = Parser::encodeRespArray(command_list).size()](const asio::error_code &ec, size_t len)
    {
      if (ec)
      {
        std::cout << "Failed to write message, error = " << ec.message() << "\n";
      }
      else
      {
        if (session_->is_master_session())
        {
          replication_info_->updateOffset(offset);
        }
      }
    };

    command->handle(command_list, session_, callback);
  }
}