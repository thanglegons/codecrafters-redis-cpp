#include "Parser.h"
#include <cctype>
#include <iostream>
#include <sstream>
#include <string>

std::vector<std::string> Parser::parse(const std::string &command) {
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