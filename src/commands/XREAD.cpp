#include "XREAD.hpp"
#include "Parser.h"
#include "Session.h"
#include <asio/steady_timer.hpp>
#include <optional>
#include <span>
#include <vector>

namespace commands {

void XREAD::handle(const std::vector<std::string> &command_list,
                   Session *session) {
  std::span<const std::string> params{command_list.begin() + 1,
                                      command_list.end()};

  std::optional<std::string> return_message = inner_handle(params, session);
}

std::optional<std::string>
XREAD::inner_handle(const std::span<const std::string> &params,
                    Session *session) {
  int num_params = params.size();
  if (num_params < 1 || num_params % 2 != 1) {
    return std::nullopt;
  }

  if (params[0] == "block") {
    try {
      auto time_ms = std::stoi(params[1]);
      auto timer =
          std::make_shared<asio::steady_timer>(session->get_io_context());
      timer->expires_after(std::chrono::milliseconds(time_ms));
      std::vector<std::string> span_cpy{params.begin() + 2, params.end()};
      timer->async_wait([timer, span_cpy = std::move(span_cpy),
                         session = session->shared_from_this(),
                         this](const asio::error_code &ec) {
        std::span<const std::string> inner_param_span{span_cpy.begin(),
                                                      span_cpy.end()};
        handle_xread(inner_param_span, session.get());
      });
    } catch (std::exception &e) {
      return std::nullopt;
    }
  } else {
    handle_xread(params, session);
  }

  return std::nullopt;
}

void XREAD::handle_xread(const std::span<const std::string> &params,
                         Session *session) {
  int num_params = params.size();
  int num_pairs = (num_params - 1) / 2;
  std::cout << "DEBUG" << num_params << " " << num_pairs << "\n";
  std::vector<std::pair<std::string, std::span<const Stream::Entry>>> results;
  for (int i = 0; i + 1 + num_pairs < num_params; ++i) {
    const auto &key = params[i + 1];
    const auto &entry_id = params[i + 1 + num_pairs];
    std::cout << key << " " << entry_id << "ssssss\n";
    auto stream = data_->get_stream(key);
    if (!stream.has_value()) {
      continue;
    }
    auto entries = stream->extract_from_exclusive(entry_id);
    if (entries.empty()) {
      continue;
    }
    results.emplace_back(key, std::move(entries));
  }
  auto return_message = results.empty() ? "$-1\r\n" : Parser::encodeRespArray(results);
  write(std::move(return_message), session);
}

} // namespace commands