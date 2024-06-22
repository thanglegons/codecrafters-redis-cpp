#include "Ping.h"
#include "Parser.h"

namespace commands {
std::optional<std::string> Ping::inner_handle() {
  return Parser::encodeString("PONG");
}
} // namespace commands