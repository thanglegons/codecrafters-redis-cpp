#include "Command.h"
#include "ReplicationInfo.h"
#include <memory>
#include <optional>
#include <string>

namespace commands {
struct Psync : public Command {
  Psync(const std::span<std::string> &params, Session *session,
        std::shared_ptr<ReplicationInfo> rep_info)
      : Command(params, session), rep_info_(rep_info) {}

  std::optional<std::string> inner_handle() override;

private:
  std::shared_ptr<ReplicationInfo> rep_info_;
};
} // namespace commands