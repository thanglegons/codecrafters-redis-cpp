#include "Command.h"
#include "Storage.h"
#include <memory>
#include <optional>
#include <string>

namespace commands {
struct Type : public Command {
  Type(const std::shared_ptr<const KVStorage> data)
      : Command(false), data_(data) {}

  ~Type() = default;

  std::optional<std::string>
  inner_handle(const std::span<const std::string> &params,
               Session *session) override;

private:
  const std::shared_ptr<const KVStorage> data_;
};
} // namespace commands