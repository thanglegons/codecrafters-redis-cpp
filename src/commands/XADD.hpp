#include "Command.h"
#include "Storage.h"
#include <memory>
#include <optional>

namespace commands {
struct XADD : public Command {
  XADD(const std::shared_ptr<KVStorage> data)
      : Command(true, true), data_(data) {}

  ~XADD() = default;

  std::optional<std::string>
  inner_handle(const std::span<const std::string> &params, Session *session);

private:
  const std::shared_ptr<KVStorage> data_;
};
} // namespace commands