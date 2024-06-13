#pragma once
#include "Storage.h"
#include <memory>

class CommandHandler {
public:
  CommandHandler(std::shared_ptr<KVStorage> data);

  std::string handle_raw_command(const std::string &raw_command);

private:
  std::shared_ptr<KVStorage> data_;
};