#include "Driver.hpp"
#include <string>


Driver::Driver(
    const std::string& fileName
) : location{&fileName}, reachedEof{false} {
    parsed = new ParsedFile{};
}

int Driver::parseFile() {
    yy::parser parser{*this};
    return parser.parse();
}

void Driver::push(Statement* statement) {
    parsed->statements.push_back(statement);
}

