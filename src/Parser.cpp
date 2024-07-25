#include "Parser.h"
#include <cctype>
#include <iostream>
#include <sstream>
#include <string>

std::vector<std::vector<std::string>>
Parser::decode(const std::string &raw_command) {
  std::istringstream iss(raw_command);
  std::string line;
  std::vector<std::vector<std::string>> commands;
  while (std::getline(iss, line)) {
    auto &command = commands.emplace_back();
    if (line[0] == '*') {
      int repeated = std::stoi(line.substr(1));
      for (int i = 0; i < repeated; i++) {
        if (!std::getline(iss, line)) {
          std::cout << "Failed to parse command = " << raw_command << "\n";
          return {};
        }
        if (line.back() == '\r') {
          line.pop_back();
        }
        if (line[0] == '$') {
          int str_len = std::stoi(line.substr(1));
          std::string bulk_str(str_len, '\0');
          iss.read(bulk_str.data(), str_len);
          command.emplace_back(std::move(bulk_str));
          iss.ignore(2);
        } else {
          // not handle
        }
      }
    } else if (line[0] == '$') {
      int str_len = std::stoi(line.substr(1));
      std::string bulk_str(str_len, '\0');
      iss.read(bulk_str.data(), str_len);
    } else {
      std::cout << "Incorrect format, command = " << raw_command << "\n";
    }
  }
  return commands;
}

std::string Parser::encodeString(const std::string &str) {
  return "+" + str + "\r\n";
}

std::string Parser::encodeBulkString(const std::string &str) {
  return "$" + std::to_string(str.size()) + "\r\n" + str + "\r\n";
}

template <>
std::string
Parser::encodeRespArray<std::string>(const std::string &str) {
  return encodeBulkString(str);
}

std::string Parser::encodeSimpleError(const std::string &str) {
  return "-" + str + "\r\n";
}