#include "Driver.hpp"
#include <string>


std::string fileName{"stdin"};

Driver::Driver() : location{&fileName}, reachedEof{false}, statements{} {
}

int Driver::parseFile() {
    yy::parser parser{*this};
    return parser.parse();
}

void Driver::push(Statement* statement) {
    statements.push_back(statement);
}

