#include "Command.h"
#include "Parser.h"
#include "Session.h"
#include <iostream>

namespace commands
{
  void Command::handle(const std::vector<std::string> &command_list,
                       Session *session,
                       std::function<void(const asio::error_code &, size_t)> callback)
  {
    std::span<const std::string> params{command_list.begin() + 1, command_list.end()};

    std::optional<std::string> return_message = inner_handle(params, session);
    if (!is_write_command || !session->is_master_session())
    {
      // don't write back if it's from master
      if (return_message.has_value())
      {
        session->write(return_message.value(), callback);
      }
      else
      {
        session->write(kErrorReturn, callback);
      }
    }
    else
    {
      callback(std::error_code(), 0);
      session->start();
    }

    after_write(params, session);

    if (is_propagate)
    {
      const auto replicas = session->get_replicas();
      for (auto &replica : replicas)
      {
        if (replica->is_session_closed())
        {
          continue;
        }
        auto raw_command = Parser::encodeRespArray(command_list);
        replica->write(
            raw_command, [](const asio::error_code &error_code, size_t len)
            {
            if (error_code) {
              std::cout << "Failed to propagate to replicas, error = "
                        << error_code.message() << std::endl;
            } });
      }
    }
  }
  void Command::default_call_back(const asio::error_code &error_code,
                                  size_t len)
  {
    if (error_code)
    {
      std::cout << "error = " << error_code.message() << "\n";
    }
  }
} // namespace commands