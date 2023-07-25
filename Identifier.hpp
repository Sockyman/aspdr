#ifndef IDENTIFIER_HPP
#define IDENTIFIER_HPP

#include <string>
#include <vector>
#include <compare>
#include <iostream>
#include <cstdint>

class Identifier {
public:
    std::vector<std::string> value;

    Identifier();
    Identifier(std::vector<std::string> value);
    Identifier(std::string str);

    void push(std::string str);
};

std::ostream& operator<<(std::ostream& stream, const Identifier& id);

std::strong_ordering operator<=>(const Identifier& id0, const Identifier& id1);

class UnqualifiedIdentifier{
public:
    Identifier identifier;
    std::size_t depth;

    UnqualifiedIdentifier();
    UnqualifiedIdentifier(std::size_t depth, Identifier identifier);

    Identifier qualify(const Identifier& id);
};

#endif

