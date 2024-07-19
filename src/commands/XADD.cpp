#include "XADD.hpp"
#include "Parser.h"

namespace commands {

std::optional<std::string>
XADD::inner_handle(const std::span<const std::string> &params,
                   Session *session) {
  int num_params = params.size();
  if (num_params < 3) {
    return std::nullopt;
  }
  auto key = params[0];
  auto id = params[1];
  std::vector<std::pair<std::string, std::string>> pairs;
  for (int i = 2; i < params.size(); i += 2) {
    pairs.emplace_back(params[i], params[i + 1]);
  }
  Stream::StreamError err = Stream::StreamError::OK;
  auto ret = data_->set_stream(std::move(key), std::move(id), std::move(pairs), err);
  if (err == Stream::StreamError::OK) {
    if (!ret.has_value()) {
      return std::nullopt;
    }
    return Parser::encodeBulkString(ret.value());
  } else if (err == Stream::StreamError::entryIDIsZero) {
    return Parser::encodeSimpleError("ERR The ID specified in XADD must be greater than 0-0");
  } else if (err == Stream::StreamError::entryIDIsEqualOrSmaller) {
    return Parser::encodeSimpleError("ERR The ID specified in XADD is equal or smaller than the target stream top item");
  } else if (err == Stream::StreamError::entryIDParsedError) {
    return Parser::encodeSimpleError("ERR Failed to parse entry_id");
  }
  return std::nullopt;
}

} // namespace commands