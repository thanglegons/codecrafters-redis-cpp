#include "Command.h"
#include "ReplicationInfo.h"
#include <memory>

namespace commands {
struct Info : public Command {
  Info(const std::span<std::string> &params, Session *session,
       const std::shared_ptr<const ReplicationInfo> replication_info)
      : Command(params, session), replication_info_(replication_info) {}

  std::optional<std::string> inner_handle() override;

private:
  const std::shared_ptr<const ReplicationInfo> replication_info_;
};
} // namespace commands