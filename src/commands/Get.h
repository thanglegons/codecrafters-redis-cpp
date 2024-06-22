#include "Command.h"
#include "Storage.h"
#include <memory>
#include <optional>
#include <string>

namespace commands {
struct Get : public Command {
  Get(const std::span<std::string> &params, Session *session,
      const std::shared_ptr<const KVStorage> data)
      : Command(params, session), data_(data) {}

  std::optional<std::string> inner_handle() override;

private:
  const std::shared_ptr<const KVStorage> data_;
};
} // namespace commands