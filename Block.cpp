#include "Block.hpp"
#include "Context.hpp"

Block::Block(std::vector<Statement*> statements)
: once{false}, statements{statements} {
}

Block::~Block() {
    for (auto& elem : this->statements) {
        delete elem;
    }
}

void Block::push(Statement* statement) {
    this->statements.push_back(statement);
}

bool Block::assemble(Context& context) {
    return context.assembler->assemble(context, this->statements);
}

