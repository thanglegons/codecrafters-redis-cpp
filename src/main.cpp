#include "Server.h"
#include <asio/io_context.hpp>
#include <exception>
#include <iostream>

struct Argument {
  uint32_t port{kDefaultPort};

  static constexpr uint32_t kDefaultPort = 6379;
};

Argument read_argument(int argc, char **argv) {
  // Command line arguments will be stored in a map
  std::unordered_map<std::string, std::string> args;

  // Parse the command line arguments
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg.substr(0, 2) == "--" && i + 1 < argc) {
      args[arg.substr(2)] = argv[++i];
    }
  }

  Argument argument;
  // Check if the port argument was provided
  if (args.find("port") != args.end()) {
    int port = std::stoi(args["port"]);
    argument.port = port;
  }

  return argument;
}

int main(int argc, char **argv) {
  // Flush after every std::cout / std::cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  // You can use print statements as follows for debugging, they'll be visible
  // when running tests.
  std::cout << "Logs from your program will appear here!\n";

  auto argument = read_argument(argc, argv);

  try {
    asio::io_context io_context;
    Server server(io_context, argument.port);
    io_context.run();
  } catch (std::exception &e) {
    std::cout << "Exception: " << e.what() << "\n";
  }

  return 0;
}
