#pragma once

#include <string>
#include <vector>
#include <span>

class Parser {
public:
  static std::vector<std::vector<std::string>>
  decode(const std::string &command);

  static std::string encodeString(const std::string &str);

  static std::string encodeBulkString(const std::string &str);

  template <typename T = std::string>
  static std::string encodeRespArray(std::vector<T>&& arr) {
    auto arr_span = std::span<T>(std::move(arr.begin()), std::move(arr.end()));
    return encodeRespArray<T>(arr_span);
  }

  template <typename T = std::string>
  static std::string encodeRespArray(const std::vector<T>& arr) {
    auto arr_span = std::span<const T>(arr);
    return encodeRespArray<T>(arr_span);
  }

  template <typename T = std::string>
  static std::string encodeRespArray(const std::span<const T>& arr) {
    std::string result = "*" + std::to_string(arr.size()) + "\r\n";
    for (const auto &str : arr) {
      result += encodeElementRespArray(str);
    }
    return result;
  }

  template <typename T>
  static std::string encodeElementRespArray(const T& e) {
    return "";
  }

  static std::string encodeSimpleError(const std::string &str);
};