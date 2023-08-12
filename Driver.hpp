#ifndef DRIVER_HPP
#define DRIVER_HPP

#include "Statement.hpp"
#include "parser.hpp"
#include <vector>
#include <string>

#define YY_DECL \
    yy::parser::symbol_type yylex(Driver& driver)
YY_DECL;

class ParsedFile {
public:
    bool once{false};
    std::vector<Statement*> statements{};
};

class Driver {
public:
    yy::location location;
    bool reachedEof;
    ParsedFile* parsed;

    Driver(const std::string& fileName);
    int parseFile();
    void push(Statement* statement);
};

#endif

