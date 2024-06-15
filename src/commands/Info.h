#include "ReplicationInfo.h"
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace commands {
struct Info {
  Info(const std::vector<std::string>::iterator begin,
       const std::vector<std::string>::iterator end,
       const std::shared_ptr<const ReplicationInfo> replication_info);

  std::optional<std::string> operator()();

private:
  const std::vector<std::string>::iterator begin;
  const std::vector<std::string>::iterator end;
  const std::shared_ptr<const ReplicationInfo> replication_info_;
};
} // namespace commands