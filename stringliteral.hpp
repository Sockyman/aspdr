#ifndef STRINGLITERAL_HPP
#define STRINGLITERAL_HPP

#include <vector>
#include <string_view>
#include <cstdint>

std::vector<char> processEscapedString(std::string_view str);

std::int64_t processEscapedChar(std::string_view str);

#endif

