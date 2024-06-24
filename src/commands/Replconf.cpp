#include "Replconf.h"
#include "Parser.h"
#include "Session.h"

namespace commands {

std::optional<std::string>
Replconf::inner_handle(const std::span<std::string> &params,
                       Session* session) {
  if (params[0] == "listening-port") {
    session->set_as_replica();
  }
  return Parser::encodeString("OK");
}
} // namespace commands