#include "Command.h"
#include "Storage.h"
#include <memory>
#include <optional>

namespace commands {
struct XREAD : public Command {
  XREAD(const std::shared_ptr<KVStorage> data)
      : Command(true, true), data_(data) {}

  ~XREAD() = default;

  std::optional<std::string>
  inner_handle(const std::span<const std::string> &params, Session *session);

private:
  const std::shared_ptr<const KVStorage> data_;
};
} // namespace commands