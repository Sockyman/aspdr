#include "Driver.hpp"
#include <string>

Driver::Driver(
    const std::string& fileName
) : location{&fileName}, reachedEof{false} {
    parsed = new Block{};
}

Driver::~Driver() {}

int Driver::parseFile() {
    yy::parser parser{*this};
    return parser.parse();
}

void Driver::push(Statement* statement) {
    this->parsed->push(statement);
}

