#include "InstructionStatement.hpp"
#include "Context.hpp"
#include "Assembler.hpp"

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

    return context.getSection().writeInteger(context, expr, data.dataSize);
}

VoidResult InstructionStatement::assembleInstruction(
    Context& context,
    const Instruction& ins
) {
    if (!context
        .assembler->instructionSet.instructionReferences
        .count(ins)
    ) {
        // TODO: Find a better solution then this garbage.
        if (ins.mode.destination == Mode::Immediate) {
            Instruction ins2{ins.name, {{Mode::ImmediateAddress}, ins.mode.source}};
            return this->assembleInstruction(context, ins2);
        }

        std::stringstream ss{};
        ss << "instruction \'" << ins << "\' does not exist";
        return context.voidError({Error::Level::Fatal, this->location, ss.str()});
    }

    std::uint8_t opcode = context.assembler->instructionSet
        .instructionReferences.at(ins);

    return context.getSection().writeByte(context, this->location, opcode)
        .then(this->assembleAddress(
            context,
            ins.mode.source,
            this->expressions[1]))
        .then(this->assembleAddress(
            context,
            ins.mode.destination,
            this->expressions[0]));
}

VoidResult InstructionStatement::assemble(Context& context) {
    return this->assembleInstruction(context, this->instruction);
}

