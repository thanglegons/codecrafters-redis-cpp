#include "Wait.h"
#include "Parser.h"
#include <optional>

namespace commands {

std::optional<std::string> Wait::inner_handle(const std::span<const std::string>& params, Session* session) {
  return ":0\r\n";
}

} // namespace commands