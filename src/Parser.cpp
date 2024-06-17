#include "Parser.h"
#include <cctype>
#include <iostream>
#include <sstream>
#include <string>

std::vector<std::string> Parser::decode(const std::string &command) {
  std::istringstream iss(command);
  std::string line;
  if (!std::getline(iss, line)) {
    std::cout << "Failed to parse command = " << command << "\n";
    return {};
  }
  std::vector<std::string> results;
  if (line[0] == '*') {
    int repeated = std::stoi(line.substr(1));
    for (int i = 0; i < repeated; i++) {
      if (!std::getline(iss, line)) {
        std::cout << "Failed to parse command = " << command << "\n";
        return {};
      }
      if (line.back() == '\r') {
        line.pop_back();
      }
      if (line[0] == '$') {
        int str_len = std::stoi(line.substr(1));
        std::string bulk_str(str_len, '\0');
        iss.read(bulk_str.data(), str_len);
        results.emplace_back(std::move(bulk_str));
        iss.ignore(2);
      } else {
        // not handle
      }
    }
  } else {
    std::cout << "Incorrect format, command = " << command << "\n";
  }
  return results;
}

std::string Parser::encodeString(const std::string &str) {
  return "+" + str + "\r\n";
}

std::string Parser::encodeBulkString(const std::string &str) {
  return "$" + std::to_string(str.size()) + "\r\n" + str + "\r\n";
}

std::string Parser::encodeRespArray(const std::vector<std::string> arr) {
  std::string result = "*" + std::to_string(arr.size()) + "\r\n";
  for (const auto &str : arr) {
    result += "$" + std::to_string(str.size()) + "\r\n" + str + "\r\n";
  }
  return result;
}