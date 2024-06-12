#include "Server.h"
#include <asio/io_context.hpp>
#include <exception>
#include <iostream>

int main(int argc, char **argv) {
  // Flush after every std::cout / std::cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  // You can use print statements as follows for debugging, they'll be visible
  // when running tests.
  std::cout << "Logs from your program will appear here!\n";

  try {
    asio::io_context io_context;
    Server server(io_context, 6379);
    io_context.run();
  } catch (std::exception& e) {
    std::cout << "Exception: " << e.what() << "\n";
  }

  return 0;
}
