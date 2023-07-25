#ifndef STATEMENT_HPP
#define STATEMENT_HPP

#include "Expression.hpp"
#include "Identifier.hpp"
#include "Location.hpp"
#include <SpdrFirmware/Instruction.hpp>
#include <SpdrFirmware/Mode.hpp>
#include <string>
#include <array>

class Context;

class Statement {
public:
    Location location;
    Statement();
    Statement(Location location);
    virtual void assemble(Context& context) = 0;
};

class InstructionStatement : public Statement {
private:
    void assembleAddress(Context& context, Address& address, Expression* expr);
public:
    Instruction instruction;
    std::array<Expression*, 2> expressions;

    InstructionStatement();
    InstructionStatement(Location location, Instruction instruction);
    InstructionStatement(Location location, Instruction instruction, Expression* expr);
    InstructionStatement(Location location, Instruction instruction, Expression* expr0, Expression* expr1);

    virtual void assemble(Context& context) override;
};

class LabelStatement : public Statement {
public:
    Identifier id;

    LabelStatement();
    LabelStatement(Location location, Identifier id);

    virtual void assemble(Context& context) override;
};

class SymbolStatement : public Statement {
public:
    Identifier id;
    Expression* expr;

    SymbolStatement();
    SymbolStatement(Location location, Identifier id, Expression* expr);

    virtual void assemble(Context& context) override;
};

#endif

