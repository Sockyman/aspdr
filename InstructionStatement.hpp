#ifndef INSTRUCTIONSTATEMENT_HPP
#define INSTRUCTIONSTATEMENT_HPP

#include "Statement.hpp"

class InstructionStatement : public Statement {
private:
    bool assembleAddress(
        Context& context,
        Size size,
        const Expression* expr
    );

    bool assembleInstruction(
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

    virtual bool assemble(Context& context) override;
};

#endif

