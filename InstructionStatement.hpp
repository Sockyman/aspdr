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
    const std::vector<Expression*> arguments;

    InstructionStatement(
        Location location,
        std::string name,
        std::vector<std::pair<Address, Expression*>> mode
    );

    virtual ~InstructionStatement() override;

    virtual bool assemble(Context& context) override;
};

#endif

