#include "Command.h"
#include <span>
#include <string>

namespace commands {
struct Echo : public Command {
  Echo(const std::span<std::string> &params, Session *session)
      : Command(params, session) {}

  std::optional<std::string> inner_handle() override;
};
} // namespace commands