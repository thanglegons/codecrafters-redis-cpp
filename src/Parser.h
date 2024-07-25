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

  template <typename T>
  static std::string encodeRespArray(const T& e) {
    return "";
  }
  
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
      result += encodeRespArray(str);
    }
    return result;
  }

  template <typename T = std::string, typename U>
  static std::string encodeRespArray(const std::vector<std::pair<T, U>>& arr) {
    std::string result = "*" + std::to_string(arr.size()) + "\r\n";
    for (const auto &pair : arr) {
      result += "*2\r\n";
      result += encodeRespArray(pair.first);
      result += encodeRespArray(pair.second);
    }
    return result;
  }

  static std::string encodeSimpleError(const std::string &str);
};