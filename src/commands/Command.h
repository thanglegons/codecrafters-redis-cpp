#pragma once

#include <asio/error_code.hpp>
#include <optional>
#include <span>
#include <string>

class Session;
namespace commands {
class Command {
public:
  Command(const std::span<std::string> &params, Session *session)
      : params_(params), session_(session){};

  void handle();

protected:
  virtual std::optional<std::string> inner_handle() = 0;

  static void default_call_back(const asio::error_code &error_code, size_t len);

  const std::span<std::string> &params_;
  Session *session_;

  inline static const std::string kErrorReturn = "$-1\r\n";
};
} // namespace commands