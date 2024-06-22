#include "Command.h"
#include "Storage.h"
#include <memory>
#include <optional>

namespace commands {
struct Set : public Command {
  Set(const std::span<std::string> &params, Session *session,
      const std::shared_ptr<KVStorage> data)
      : Command(params, session), data_(data) {}

  std::optional<std::string> inner_handle();

private:
  const std::shared_ptr<KVStorage> data_;
};
} // namespace commands