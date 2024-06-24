#include "Command.h"
#include <optional>
#include <string>

namespace commands {
struct Replconf : public Command {
  Replconf() : Command(false) {}

  ~Replconf() = default;

  std::optional<std::string>
  inner_handle(const std::span<std::string> &params,
               Session* session) override;
};
} // namespace commands