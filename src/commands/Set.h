#include "Storage.h"
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace commands {
struct Set {
  Set(const std::vector<std::string>::iterator begin,
      const std::vector<std::string>::iterator end,
      const std::shared_ptr<KVStorage> data);

  std::optional<std::string> operator()();

private:
  const std::vector<std::string>::iterator begin;
  const std::vector<std::string>::iterator end;
  const std::shared_ptr<KVStorage> data;
};
} // namespace commands