#ifndef DRIVER_HPP
#define DRIVER_HPP

#include "Statement.hpp"
#include "parser.hpp"
#include <vector>
#include <string>

#define YY_DECL \
    yy::parser::symbol_type yylex(Driver& driver)
YY_DECL;

class Driver {
public:
    Driver();
    int parseFile();
    void push(Statement* statement);

    yy::location location;
    bool reachedEof;
    std::vector<Statement*> statements;
};

#endif

