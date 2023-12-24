#include "InstructionStatement.hpp"
#include "Context.hpp"
#include "Assembler.hpp"

InstructionStatement::InstructionStatement(
    Location location,
    Instruction instruction
) : InstructionStatement{location, instruction, nullptr} {}

InstructionStatement::InstructionStatement(
    Location location,
    Instruction instruction,
    Expression* expr
) : InstructionStatement{location, instruction, expr, nullptr} {}

InstructionStatement::InstructionStatement(
    Location location,
    Instruction instruction,
    Expression* expr0,
    Expression* expr1
) : Statement{location}, instruction{instruction}, expressions{expr0, expr1} {}


InstructionStatement::~InstructionStatement() {
    for (auto expr : this->expressions) {
        delete expr;
    }
}

VoidResult InstructionStatement::assemble(Context& context) {
    return context.getSection().writeInstruction(context, this->location, this->instruction, {this->expressions});
    //return this->assembleInstruction(context, this->instruction);
}

