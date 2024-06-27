#include "stringliteral.hpp"
#include <string>

char nextChar(std::string_view& str) {
    if (str.length() <= 0) {
        return '\0';
    }
    char c = str[0];
    str.remove_prefix(1);
    return c;
}

char processHex(std::string_view& str) {
    std::string s{};
    s.push_back(nextChar(str));
    s.push_back(nextChar(str));
    return std::stoi(s, nullptr, 16);
}

char processEscapeSequence(std::string_view& str) {
    char c = nextChar(str);
    switch (c) {
        case '0':
            return '\0';
        case 'n':
            return '\n';
        case 't':
            return '\t';
        case 'x':
            return processHex(str);
        default:
            return c;
    }
}

std::vector<char> processEscapedString(std::string_view str) {
    std::vector<char> result{};
    result.reserve(str.length());
    while (str.length() > 0) {
        char c = nextChar(str);
        if (c == '\\') {
            c = processEscapeSequence(str);
        }
        result.push_back(c);
    }
    return result;
}

std::int64_t processEscapedChar(std::string_view str) {
    std::uint64_t ichar = 0;
    int shift = 0;
    while (str.length() > 0) {
        char c = nextChar(str);
        if (c == '\\') {
            c = processEscapeSequence(str);
        }
        ichar |= c << (8 * shift++);
    }
    return ichar;
}

