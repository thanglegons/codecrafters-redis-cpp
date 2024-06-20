#include "ReplicationInfo.h"
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace commands {
struct Psync {
  Psync(const std::vector<std::string>::iterator begin,
        const std::vector<std::string>::iterator end,
        std::shared_ptr<ReplicationInfo> rep_info);

  std::optional<std::string> operator()();

private:
  const std::vector<std::string>::iterator begin;
  const std::vector<std::string>::iterator end;
  std::shared_ptr<ReplicationInfo> rep_info;
};
} // namespace commands