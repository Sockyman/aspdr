#ifndef BLOCK_HPP
#define BLOCK_HPP

#include "Statement.hpp"
#include <vector>

class Block {
public:
    bool once;
    std::vector<Statement*> statements;

    Block(std::vector<Statement*> statements = {});
    ~Block();

    void push(Statement* statement);
    bool assemble(Context& context);
};

#endif

