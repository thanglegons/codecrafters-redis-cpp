#include "Command.h"
#include <optional>
#include <string>
#include <iostream>

namespace commands {
struct Ping : public Command {
  Ping() : Command(true) {}

  ~Ping() = default;

  std::optional<std::string>
  inner_handle(const std::span<const std::string> &params,
               Session* session) override;
};
} // namespace commands