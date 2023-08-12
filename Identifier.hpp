#ifndef IDENTIFIER_HPP
#define IDENTIFIER_HPP

#include "Result.hpp"
#include <string>
#include <vector>
#include <compare>
#include <iostream>
#include <cstdint>

class Context;

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

    static UnqualifiedIdentifier fromString(const std::string& str);

    UnqualifiedIdentifier();
    UnqualifiedIdentifier(std::size_t depth, Identifier identifier);


    Result<Identifier> qualify(
        Context& context,
        const Location& location,
        const Identifier& id
    ) const;

    void incrementDepth();
};

std::ostream& operator<<(std::ostream& stream, const UnqualifiedIdentifier& id);

#endif

