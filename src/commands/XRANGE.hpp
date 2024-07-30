#include "Command.h"
#include "Storage.h"
#include <memory>
#include <optional>

namespace commands {
struct XRANGE : public Command {
  XRANGE(const std::shared_ptr<KVStorage> data)
      : Command(false), data_(data) {}

  ~XRANGE() = default;

  std::optional<std::string>
  inner_handle(const std::span<const std::string> &params, Session *session);

private:
  const std::shared_ptr<const KVStorage> data_;
};
} // namespace commands