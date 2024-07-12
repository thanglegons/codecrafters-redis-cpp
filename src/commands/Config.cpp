#include "Config.hpp"
#include "Parser.h"
#include "ServerConfig.hpp"
#include <iostream>

namespace commands {

std::optional<std::string>
Config::inner_handle(const std::span<const std::string> &params,
                     Session *session) {
  if (params.empty()) {
    return std::nullopt;
  }
  if (params[0] == "GET") {
    if (params.size() < 2) {
      return std::nullopt;
    }
    auto &request_key = params[1];
    if (request_key == "dir") {
      auto &rdb_info = server_config_->rdb_info;
      if (!rdb_info.has_value()) {
        return std::nullopt;
      }
      return Parser::encodeRespArray({"dir", rdb_info->dir});
    } else if (request_key == "dbfilename") {
      auto &rdb_info = server_config_->rdb_info;
      if (!rdb_info.has_value()) {
        return std::nullopt;
      }
      return Parser::encodeRespArray(
          {"dbfilename", server_config_->rdb_info->dbfilename});
    }
  }
  return std::nullopt;
}
} // namespace commands