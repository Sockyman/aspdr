#include "Statement.hpp"
#include "Assembler.hpp"
#include "Error.hpp"
#include <stdexcept>

Statement::Statement() : location{} {}

Statement::Statement(Location location) : location{location} {}

InstructionStatement::InstructionStatement() : expressions{} {}

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

VoidResult InstructionStatement::assembleAddress(
    Context& context,
    const Address& address,
    const Expression* expr
) {
    const ModeData& data = ModeData::get(address.mode);
    if (!data.dataSize) {
        return VoidResult{};
    }

    return context.section.writeInteger(context, expr, data.dataSize);
}

VoidResult InstructionStatement::assemble(Context& context) {
    if (!context
        .assembler->instructionSet.instructionReferences
        .count(this->instruction)
    ) {
        std::stringstream ss{};
        ss << "instruction \'" << this->instruction << "\' does not exist.";
        return VoidResult{{Error::Level::Fatal, this->location, ss.str()}};
    }

    std::uint8_t opcode = context.assembler->instructionSet
        .instructionReferences.at(this->instruction);

    return context.section.writeByte(this->location, opcode)
        .then(this->assembleAddress(
            context,
            this->instruction.mode.source,
            this->expressions[1]))
        .then(this->assembleAddress(
            context,
            this->instruction.mode.destination,
            this->expressions[0]));
}


LabelStatement::LabelStatement() {}

LabelStatement::LabelStatement(Location location, Identifier id)
: Statement{location}, id{id} {}

VoidResult LabelStatement::assemble(Context& context) {
    auto address = context.section.getAddress();
    if (!address) {
        return {{Error::Level::Pass, this->location, "cannot get address"}};
    }

    return context.assembler->assignSymbol(
        this->location,
        this->id,
        *address
    );
}


SymbolStatement::SymbolStatement() {}

SymbolStatement::SymbolStatement(
    Location location,
    Identifier id,
    Expression* expr
) : Statement{location}, id{id}, expr{expr} {}

VoidResult SymbolStatement::assemble(Context& context) {
    Result<std::int64_t> result = this->expr->evaluate(context);
    if (result.isErr()) {
        return result.into<VoidResult>();
    }

    return context.assembler->assignSymbol(this->location, this->id, result.getOk());
}

