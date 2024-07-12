#include "Command.h"
#include <memory>

class ServerConfig;

namespace commands {
struct Config: public Command {
  Config(const std::shared_ptr<const ServerConfig> server_config)
      : Command(false), server_config_(server_config) {}

  ~Config() = default;

  std::optional<std::string>
  inner_handle(const std::span<const std::string> &params,
               Session *session) override;

private:
  const std::shared_ptr<const ServerConfig> server_config_;
};
} // namespace commands