#ifndef INSTRUCTIONSTATEMENT_HPP
#define INSTRUCTIONSTATEMENT_HPP

#include "Statement.hpp"

class InstructionStatement : public Statement {
private:
    VoidResult assembleAddress(
        Context& context,
        const Address& address,
        const Expression* expr);

    VoidResult assembleInstruction(
        Context& context,
        const Instruction& ins
    );

public:
    const Instruction instruction;
    const std::array<Expression*, 2> expressions;

    InstructionStatement(Location location, Instruction instruction);

    InstructionStatement(
        Location location,
        Instruction instruction,
        Expression* expr);

    InstructionStatement(
        Location location,
        Instruction instruction,
        Expression* expr0,
        Expression* expr1);

    virtual ~InstructionStatement() override;

    virtual VoidResult assemble(Context& context) override;
};

#endif

