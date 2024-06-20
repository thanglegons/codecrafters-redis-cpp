#include "Psync.h"
#include "Parser.h"
#include "ReplicationInfo.h"
#include <memory>

namespace commands {

Psync::Psync(const std::vector<std::string>::iterator begin,
             const std::vector<std::string>::iterator end,
             std::shared_ptr<ReplicationInfo> rep_info)
    : begin(begin), end(end), rep_info(rep_info) {}

std::optional<std::string> Psync::operator()() {
    std::string response = "FULLRESYNC " + rep_info->master_replid + " 0";
    return Parser::encodeString(response);
}
} // namespace commands