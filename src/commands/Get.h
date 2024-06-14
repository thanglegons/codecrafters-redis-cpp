#include "Storage.h"
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace commands {
struct Get {
  Get(const std::vector<std::string>::iterator begin,
      const std::vector<std::string>::iterator end,
      const std::shared_ptr<const KVStorage> data);

  std::optional<std::string> operator()();

private:
  const std::vector<std::string>::iterator begin;
  const std::vector<std::string>::iterator end;
  const std::shared_ptr<const KVStorage> data;
};
} // namespace commands