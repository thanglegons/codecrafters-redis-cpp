#include "RDB.hpp"
#include "Storage.h"
#include "rdb/Utils.hpp"
#include <cassert>
#include <fstream>
#include <iostream>

void read_sub_db(std::stringstream &ss, std::shared_ptr<KVStorage> &storage) {
  auto index = decode_size(ss).first.value();
  auto flag = static_cast<uint8_t>(read(ss));
  assert(flag == 0xFB);
  auto non_expire_sz = decode_size(ss).first.value();
  auto expire_sz = decode_size(ss).first.value();
  for (int i = 0; i < non_expire_sz; i++) {
    flag = static_cast<uint8_t>(read(ss));
    if (flag == 0xFC) {
      auto timestamp_ms = read<int64_t>(ss);
      auto encoding = static_cast<uint8_t>(read(ss));
      if (encoding == 0) {
        auto key = decode_string(ss);
        auto value = decode_string(ss);
        storage->set(std::move(key), std::move(value), timestamp_ms);
      }
    } else if (flag == 0xFD) {
      auto timestamp_s = read<int64_t>(ss);
      auto encoding = static_cast<uint8_t>(read(ss));
      if (encoding == 0) {
        auto key = decode_string(ss);
        auto value = decode_string(ss);
        storage->set(std::move(key), std::move(value), timestamp_s * 1000);
      }
    } else {
      uint8_t encoding = flag;
      if (encoding == 0) {
        auto key = decode_string(ss);
        auto value = decode_string(ss);
        storage->set(std::move(key), std::move(value));
      }
    }
  }
}

auto RDB::decode(const std::string &path) -> std::shared_ptr<KVStorage> {
  static constexpr size_t kHeaderSize = 9;
  std::ifstream fi(path);
  std::stringstream ss;
  ss << fi.rdbuf();
  fi.close();
  std::string header(kHeaderSize, '\0');
  ss.read(header.data(), kHeaderSize);
  auto storage = std::make_shared<KVStorage>();
  while (ss) {
    auto rbyte = static_cast<uint8_t>(read<char>(ss));
    if (rbyte == 0xFA) {
      auto key = decode_string(ss);
      auto value = decode_string(ss);
      storage->update_info(std::move(key), std::move(value));
    } else if (rbyte == 0xFE) {
      read_sub_db(ss, storage);
    } else if (rbyte == 0xFF) {
      auto checksum = read<int64_t>(ss);
      std::cout << "Check sum = " << checksum << "\n";
      break;
    }
  }
  return storage;
}