#include "InstructionStatement.hpp"
#include "Context.hpp"
#include "Assembler.hpp"

//InstructionStatement::InstructionStatement(
//    Location location,
//    std::string name,
//    const std::vector<std::pair<Address, Expression*>>& mode
//) : Statement{location} {
//
//}


InstructionStatement::InstructionStatement(
    Location location,
    std::string name,
    std::vector<std::pair<Address, Expression*>> mode
) 
    : Statement{location},
    instruction{getInstruction(name, mode)}, arguments{getSecond(mode)} {
}

InstructionStatement::~InstructionStatement() {
    for (auto expr : this->arguments) {
        delete expr;
    }
}

bool InstructionStatement::assemble(Context& context) {
    return context
        .getSection()
        .writeInstruction(
            context,
            this
        );
    //return this->assembleInstruction(context, this->instruction);
}

