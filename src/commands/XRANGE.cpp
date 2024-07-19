#include "XRANGE.hpp"
#include "Parser.h"
#include <optional>

namespace commands {

std::optional<std::string>
XRANGE::inner_handle(const std::span<const std::string> &params,
                     Session *session) {
  int num_params = params.size();
  if (num_params < 1) {
    return std::nullopt;
  }
  const auto &key = params[0];
  auto stream = data_->get_stream(key);
  if (!stream.has_value()) {
    return std::nullopt;
  }
  auto entry_span = stream->extract_range(params[1], params[2]);
  return Parser::encodeRespArray<Stream::Entry>(entry_span);
}

} // namespace commands