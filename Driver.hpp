#ifndef DRIVER_HPP
#define DRIVER_HPP

#include "Statement.hpp"
#include "parser.hpp"
#include "Block.hpp"
#include <string>

#define YY_DECL \
    yy::parser::symbol_type yylex(Driver& driver)
YY_DECL;

class Driver {
public:
    yy::location location;
    bool reachedEof;
    Block* parsed;
    //std::unique_ptr<ParsedFile> parsed;

    Driver(const std::string& fileName);
    ~Driver();

    int parseFile();
    void push(Statement* statement);
};

#endif

