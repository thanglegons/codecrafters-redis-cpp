#include "XREAD.hpp"
#include "Parser.h"
#include <optional>

namespace commands {

std::optional<std::string>
XREAD::inner_handle(const std::span<const std::string> &params,
                    Session *session) {
  int num_params = params.size();
  if (num_params < 1 || num_params % 2 != 1) {
    return std::nullopt;
  }
  int num_pairs = (num_params - 1) / 2;
  std::vector<std::pair<std::string, std::span<const Stream::Entry>>> results;
  for (int i = 0; i + 1 + num_pairs < num_params; ++i) {
    const auto &key = params[i + 1];
    const auto &entry_id = params[i + 1 + num_pairs];
    auto stream = data_->get_stream(key);
    if (!stream.has_value()) {
      continue;
    }
    auto entries = stream->extract_from_exclusive(entry_id);
    results.emplace_back(key, std::move(entries));
  }
  return Parser::encodeRespArray(results);
}

} // namespace commands