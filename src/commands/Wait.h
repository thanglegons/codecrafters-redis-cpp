#include "Command.h"
#include "Storage.h"
#include <memory>
#include <optional>
#include <string>

namespace commands
{
    struct Wait : public Command
    {
        Wait()
            : Command(false) {}

        ~Wait() = default;

        std::optional<std::string>
        inner_handle(const std::span<const std::string> &params,
                     Session *session) override;
    };
} // namespace commands