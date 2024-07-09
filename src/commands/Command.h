#pragma once

#include <asio/error_code.hpp>
#include <optional>
#include <span>
#include <string>
#include <vector>
#include <functional>

class Session;
namespace commands
{
  class Command
  {
  public:
    Command(bool is_write_command, bool is_propagate = false) : is_write_command(is_write_command), is_propagate{is_propagate} {};

    virtual ~Command() = default;

    void handle(const std::vector<std::string> &command_list, Session *session,
                std::function<void(const asio::error_code &, size_t)> callback);

  protected:
    virtual std::optional<std::string> inner_handle(const std::span<const std::string> &params, Session *session) = 0;

    virtual void after_write(const std::span<const std::string> &params, Session *session)
    {
      return;
    }

    static void default_call_back(const asio::error_code &error_code, size_t len);

    bool is_write_command{false};
    bool is_propagate{false};

    inline static const std::string kErrorReturn = "$-1\r\n";
  };
} // namespace commands