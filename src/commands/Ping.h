#include <optional>
#include <string>

namespace commands {
struct Ping {
  std::optional<std::string> operator()();
};
} // namespace commands