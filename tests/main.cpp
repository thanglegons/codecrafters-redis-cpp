#include <fstream>
#include <gtest/gtest.h>
#include <iomanip>
#include <iostream>
#include <rdb/Utils.hpp>

TEST(TestDecodeString, test_decode_string) {
  {
    std::vector<int> vt = {0x0D, 0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x2C,
                           0x20, 0x57, 0x6F, 0x72, 0x6C, 0x64, 0x21};
    std::stringstream ss;
    for (auto &x : vt) {
      ss << (u_char)x;
    }
    auto result = decode_string(ss);
    EXPECT_EQ(result, "Hello, World!");
  }
  {
    std::stringstream ss;
    ss << (u_char)0xC0 << (u_char)0x7B;
    auto result = decode_string(ss);
    EXPECT_EQ(result, "123");
  }
  {
    std::stringstream ss;
    ss << (u_char)0xC1 << (u_char)0x39 << (u_char)0x30;
    auto result = decode_string(ss);
    EXPECT_EQ(result, "12345");
  }
  {
    std::stringstream ss;
    ss << (u_char)0xC2 << (u_char)0x87 << (u_char)0xD6 << (u_char)0x12
       << (u_char)0x00;
    auto result = decode_string(ss);
    EXPECT_EQ(result, "1234567");
  }
}

TEST(TestDecodeSize, test_decode_size) {
  {
    int a = 0b00001010;
    std::stringstream ss;
    ss << (u_char)a;
    auto result = decode_size(ss);
    EXPECT_EQ(result.first.value(), 10);
  }
  {
    std::stringstream ss;
    int a = 0b01000010;
    int b = 0b10111100;
    ss << (u_char)a << (u_char)b;
    auto result = decode_size(ss);
    EXPECT_EQ(result.first.value(), 700);
  }
  {
    std::stringstream ss;
    int a = 0b10000000;
    int b = 0b10000000;
    int c = 0b10000000;
    int d = 0b01000010;
    int e = 0b01101000;
    // 10000000100000000100001001101000
    ss << (u_char)a << (u_char)b << (u_char)c << (u_char)d << (u_char)e;
    auto result = decode_size(ss);
    EXPECT_EQ(result.first.value(), 0b10000000100000000100001001101000);
    std::cout << result.first.value() << "\n";
  }
  {
    std::stringstream ss;
    int a = 0xC0;
    ss << (u_char)a;
    auto result = decode_size(ss);
    EXPECT_EQ(result.second.value(), 1);
  }
  {
    std::stringstream ss;
    int a = 0xC1;
    ss << (u_char)a;
    auto result = decode_size(ss);
    EXPECT_EQ(result.second.value(), 2);
  }
  {
    std::stringstream ss;
    int a = 0xC2;
    ss << (u_char)a;
    auto result = decode_size(ss);
    EXPECT_EQ(result.second.value(), 4);
  }
}

// Main function for Google Test
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}