#include "Replconf.h"
#include "Parser.h"

namespace commands {

std::optional<std::string> Replconf::inner_handle() {
    return Parser::encodeString("OK");
}
} // namespace commands