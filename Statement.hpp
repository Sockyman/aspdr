#ifndef STATEMENT_HPP
#define STATEMENT_HPP

#include "Expression.hpp"
#include "Identifier.hpp"
#include "Location.hpp"
#include "Result.hpp"
#include <SpdrFirmware/Instruction.hpp>
#include <SpdrFirmware/Mode.hpp>
#include <string>
#include <array>

class Context;

class Statement {
public:
    const Location location;

    Statement();
    Statement(Location location);
    virtual VoidResult assemble(Context& context) = 0;
};

class InstructionStatement : public Statement {
private:
    VoidResult assembleAddress(
        Context& context,
        const Address& address,
        const Expression* expr);

public:
    const Instruction instruction;
    const std::array<Expression*, 2> expressions;

    InstructionStatement();
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

    virtual VoidResult assemble(Context& context) override;
};

class LabelStatement : public Statement {
public:
    const Identifier id;

    LabelStatement();
    LabelStatement(Location location, Identifier id);

    virtual VoidResult assemble(Context& context) override;
};

class SymbolStatement : public Statement {
public:
    const Identifier id;
    const Expression* expr;

    SymbolStatement();
    SymbolStatement(Location location, Identifier id, Expression* expr);

    virtual VoidResult assemble(Context& context) override;
};

#endif

