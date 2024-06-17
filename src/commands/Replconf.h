#include <optional>
#include <string>
#include <vector>

namespace commands {
struct Replconf {
  Replconf(const std::vector<std::string>::iterator begin,
           const std::vector<std::string>::iterator end);

  std::optional<std::string> operator()();

private:
  const std::vector<std::string>::iterator begin;
  const std::vector<std::string>::iterator end;
};
} // namespace commands