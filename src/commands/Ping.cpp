#include "Ping.h"
#include "Parser.h"

namespace commands {
std::optional<std::string> Ping::operator()() {
  return Parser::encodeString("PONG");
}
} // namespace commands