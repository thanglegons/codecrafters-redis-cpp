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
      auto timer =
          std::make_shared<asio::steady_timer>(session->get_io_context());
      std::vector<std::string> span_cpy;
      for (auto it = params.begin() + 2; it != params.end(); it++) {
        if (*it == "$") {
          const auto &key = *std::prev(it);
          auto stream = data_->get_stream(key);
          if (!stream.has_value()) {
            span_cpy.emplace_back("0-1");
          } else {
            span_cpy.emplace_back(stream->get_last_entry_id().to_string());
          }
        } else {
          span_cpy.emplace_back(*it);
        }
      }
      auto time_ms = std::stoi(params[1]);
      if (time_ms == 0) {
        // blocking no timeout
        auto result = handle_xread(params.subspan(2), session);
        if (!result.empty()) {
          // if there are anything to return, no need to wait
          parse_and_write(result, session);
        } else {
          timer->expires_after(std::chrono::hours(24 * 365 * 100));
          timer->async_wait([span_cpy = std::move(span_cpy),
                             session = session->shared_from_this(),
                             this](const asio::error_code &ec) {
            std::span<const std::string> inner_param_span{span_cpy.begin(),
                                                          span_cpy.end()};
            auto result = handle_xread(inner_param_span, session.get());
            parse_and_write(result, session.get());
          });
          int num_pairs = (params.size() - 3) / 2;
          for (int i = 0; i < num_pairs; i++) {
            const auto &key = params[i + 3];
            data_->add_waiting_timers(key, timer);
            std::cout << "Add waiting timer for key = " << key << "\n";
          }
        }
        return std::nullopt;
      }
      timer->expires_after(std::chrono::milliseconds(time_ms));
      timer->async_wait([timer, span_cpy = std::move(span_cpy),
                         session = session->shared_from_this(),
                         this](const asio::error_code &ec) {
        std::span<const std::string> inner_param_span{span_cpy.begin(),
                                                      span_cpy.end()};
        auto result = handle_xread(inner_param_span, session.get());
        parse_and_write(result, session.get());
      });
    } catch (std::exception &e) {
      return std::nullopt;
    }
  } else {
    auto result = handle_xread(params, session);
    parse_and_write(result, session);
  }

  return std::nullopt;
}

XREAD::Result XREAD::handle_xread(const std::span<const std::string> &params,
                                  Session *session) {
  int num_params = params.size();
  int num_pairs = (num_params - 1) / 2;
  Result results;
  for (int i = 0; i + 1 + num_pairs < num_params; ++i) {
    const auto &key = params[i + 1];
    const auto &entry_id = params[i + 1 + num_pairs];
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
  return results;
}

void XREAD::parse_and_write(const XREAD::Result &result, Session *session) {
  auto write_msg = result.empty() ? "$-1\r\n" : Parser::encodeRespArray(result);
  write(std::move(write_msg), session);
}

} // namespace commands