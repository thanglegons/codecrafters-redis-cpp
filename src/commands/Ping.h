#include "Command.h"
#include <optional>
#include <string>

namespace commands {
struct Ping : public Command {
  Ping(const std::span<std::string> &params, Session *session)
      : Command(params, session) {}

  std::optional<std::string> inner_handle() override;
};
} // namespace commands