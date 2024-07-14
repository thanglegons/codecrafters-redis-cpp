#include <optional>
#include <sstream>
#include <cassert>

template <typename T = char> T read(std::stringstream &ss) {
  T val;
  ss.read((char *)&val, sizeof(val));
  return val;
}

uint8_t get_x_sig_bit(uint8_t num, uint8_t x) {
  static constexpr uint8_t kNumBit = 8;
  assert(x <= kNumBit);
  return num >> (kNumBit - x);
}

uint8_t get_x_least_sig_bit(uint8_t num, uint8_t x) {
  static constexpr uint8_t kNumBit = 8;
  assert(x <= kNumBit);
  return num & ((1 << x) - 1);
}

std::pair<std::optional<uint32_t>, std::optional<int8_t>>
decode_size(std::stringstream &ss) {
  auto byte = read(ss);
  auto sig = get_x_sig_bit(byte, 2);
  if (sig == 0) {
    return {get_x_least_sig_bit(byte, 6), std::nullopt};
  } else if (sig == 1) {
    auto last_6_bit = get_x_least_sig_bit(byte, 6);
    auto next_byte = static_cast<uint8_t>(read<char>(ss));
    uint32_t sz = (last_6_bit << 8) | next_byte;
    return {sz, std::nullopt};
  } else if (sig == 2) {
    uint32_t sz = 0;
    for (int i = 0; i < 4; i++) {
      auto byte = read(ss);
      sz = (sz << 8) | (static_cast<uint8_t>(byte));
    }
    return {sz, std::nullopt};
  }
  switch (static_cast<uint8_t>(byte)) {
  case 0xC0:
    return {std::nullopt, 1};
  case 0xC1:
    return {std::nullopt, 2};
  case 0xC2:
    return {std::nullopt, 4};
  }
  return {std::nullopt, 0};
}

std::string decode_string(std::stringstream &ss) {
  auto decoded_size = decode_size(ss);
  if (decoded_size.first.has_value()) {
    int sz = decoded_size.first.value();
    std::string str(sz, '\0');
    ss.read(str.data(), sz);
    return str;
  }
  assert(decoded_size.second.has_value());
  int type = decoded_size.second.value();
  if (type == 1) {
    auto val = read<int8_t>(ss);
    return std::to_string(val);
  } else if (type == 2) {
    auto val = read<int16_t>(ss);
    return std::to_string(val);
  } else if (type == 4) {
    auto val = read<int32_t>(ss);
    return std::to_string(val);
  }
}