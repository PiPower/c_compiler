#pragma once
#include <inttypes.h>
#include <string_view>
constexpr uint8_t MODE_DEC = 0;
constexpr uint8_t MODE_HEX = 1;
constexpr uint8_t MODE_OCT = 2;
constexpr uint8_t MODE_BIN = 3;

double stringToDouble(const char* data, int32_t len, uint8_t mode);
int64_t stringToInt64(const char* data, int32_t len, uint8_t mode);
int64_t stringToInt64(const std::string_view& view, uint8_t mode);
int64_t stringToChar(const char* data, int32_t len);