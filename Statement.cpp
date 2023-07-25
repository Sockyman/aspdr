#include "Statement.hpp"
#include "Assembler.hpp"
#include "Error.hpp"
#include <stdexcept>

Statement::Statement() : location{} {
}

Statement::Statement(Location location) : location{location} {
}

InstructionStatement::InstructionStatement() {}

InstructionStatement::InstructionStatement(Location location, Instruction instruction)
    : InstructionStatement{location, instruction, nullptr} {}

InstructionStatement::InstructionStatement(Location location, Instruction instruction, Expression* expr)
    : InstructionStatement{location, instruction, expr, nullptr} {}

InstructionStatement::InstructionStatement(
    Location location,
    Instruction instruction,
    Expression* expr0,
    Expression* expr1
) : Statement{location}, instruction{instruction}, expressions{expr0, expr1} {

}

void InstructionStatement::assembleAddress(Context& context, Address& address, Expression* expr) {
    const ModeData& data = ModeData::get(address.mode);

    if (!data.dataSize)
        return;

    std::int64_t value = expr->evaluate(context);

    if (data.dataSize == 1)
        context.section.writeByte(value);
    else if (data.dataSize == 2)
        context.section.writeWord(value);
}

void InstructionStatement::assemble(Context& context) {
    if (!context.assembler->instructionSet.instructionReferences.count(this->instruction)) {
        std::stringstream ss{};
        ss << "instruction \'" << this->instruction << "\' does not exist.";
        throw FatalError{this->location, ss.str()};
    }

    std::uint8_t opcode = context.assembler->instructionSet.instructionReferences[this->instruction];
    context.section.writeByte(opcode);
    this->assembleAddress(context, this->instruction.mode.source, this->expressions[1]);
    this->assembleAddress(context, this->instruction.mode.destination, this->expressions[0]);
}


LabelStatement::LabelStatement() {}

LabelStatement::LabelStatement(Location location, Identifier id) : Statement{location}, id{id} {}

void LabelStatement::assemble(Context& context) {
    context.assembler->assignSymbol(this->id, context.section.getAddress());
}


SymbolStatement::SymbolStatement() {}

SymbolStatement::SymbolStatement(Location location, Identifier id, Expression* expr)
: Statement{location}, id{id}, expr{expr} {}

void SymbolStatement::assemble(Context& context) {
    context.assembler->assignSymbol(this->id, this->expr->evaluate(context));
}

