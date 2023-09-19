#include "Driver.hpp"
#include <string>


Driver::Driver(
    const std::string& fileName
) : location{&fileName}, reachedEof{false} {
    parsed = new ParsedFile{};
}

Driver::~Driver() {}

int Driver::parseFile() {
    yy::parser parser{*this};
    return parser.parse();
}

void Driver::push(Statement* statement) {
    parsed->statements.push_back(statement);
}

ParsedFile::ParsedFile() : once{false}, statements{} {
}

ParsedFile::~ParsedFile() {
    for (auto& elem : this->statements) {
        delete elem;
    }
}

