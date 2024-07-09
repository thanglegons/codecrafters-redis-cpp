#include "Replconf.h"
#include "Parser.h"
#include "Session.h"

namespace commands {

std::optional<std::string>
Replconf::inner_handle(const std::span<const std::string> &params,
                       Session* session) {
  if (params[0] == "GETACK") {
    return Parser::encodeRespArray(std::vector<std::string>{"REPLCONF", "ACK", "0"});
  }
  return Parser::encodeString("OK");
}
} // namespace commands