#ifndef DRIVER_HPP
#define DRIVER_HPP

#include "Statement.hpp"
#include "parser.hpp"
#include <memory>
#include <vector>
#include <string>

#define YY_DECL \
    yy::parser::symbol_type yylex(Driver& driver)
YY_DECL;

class ParsedFile {
public:
    bool once;
    std::vector<Statement*> statements;

    ParsedFile();
    ~ParsedFile();
};

class Driver {
public:
    yy::location location;
    bool reachedEof;
    ParsedFile* parsed;
    //std::unique_ptr<ParsedFile> parsed;

    Driver(const std::string& fileName);
    ~Driver();

    int parseFile();
    void push(Statement* statement);
};

#endif

