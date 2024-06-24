#pragma once

#include <asio/error_code.hpp>
#include <optional>
#include <span>
#include <string>

class Session;
namespace commands {
class Command {
public:
  Command(bool is_write_command) : is_write_command(is_write_command){};

  virtual ~Command() = default;

  void handle(std::string raw_command, Session* session);

protected:
  virtual std::optional<std::string> inner_handle(const std::span<std::string>& params, Session* session) = 0;

  virtual void after_write(const std::span<std::string>& params, Session* session) {
    return;
  }

  static void default_call_back(const asio::error_code &error_code, size_t len);

  bool is_write_command{false};

  inline static const std::string kErrorReturn = "$-1\r\n";
};
} // namespace commands