#include "Replconf.h"
#include "Parser.h"

namespace commands {

Replconf::Replconf(const std::vector<std::string>::iterator begin,
                   const std::vector<std::string>::iterator end)
    : begin(begin), end(end) {}

std::optional<std::string> Replconf::operator()() {
    return Parser::encodeString("OK");
}
} // namespace commands