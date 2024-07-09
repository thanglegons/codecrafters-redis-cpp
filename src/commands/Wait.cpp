#include "Wait.h"
#include "Parser.h"
#include "Session.h"
#include <optional>

namespace commands
{

    std::optional<std::string> Wait::inner_handle(const std::span<const std::string> &params, Session *session)
    {
        auto replicas = session->get_replicas();
        return ":" + std::to_string(replicas.size()) + "\r\n";
    }

} // namespace commands