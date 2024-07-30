#include "Command.h"
#include "Storage.h"
#include <memory>
#include <optional>

namespace commands {
struct XREAD : public Command {
  using Result =
      std::vector<std::pair<std::string, std::span<const Stream::Entry>>>;

  XREAD(const std::shared_ptr<KVStorage> data) : Command(false), data_(data) {}

  ~XREAD() = default;

  void handle(const std::vector<std::string> &command_list,
              Session *session) override;

  std::optional<std::string>
  inner_handle(const std::span<const std::string> &params,
               Session *session) override;

  Result handle_xread(const std::span<const std::string> &params,
                      Session *session);

  void parse_and_write(const Result& result, Session* session);

private:
  const std::shared_ptr<const KVStorage> data_;
};
} // namespace commands