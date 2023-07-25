#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include "Identifier.hpp"
#include "Location.hpp"
#include <cstdint>
#include <memory>
#include <exception>

class Context;

class Expression {
private:
public:
    Location location;
    Expression(Location location);
    virtual std::int64_t evaluate(Context& context) = 0;
};

enum class Binary {
    Add,
    Subtract,
    Multiply,
    Divide,
    Modulo,
    ShiftLeft,
    ShiftRight,
    BinAnd,
    BinOr,
    BinXor,
    And,
    Or,
    Greater,
    Less,
    GreaterEqual,
    LessEqual,
    Equal,
    NotEqual,
};

class BinaryExpression: public Expression {
private:
    Binary operation;
    Expression* operand0;
    Expression* operand1;
public:
    BinaryExpression(Location location, Binary operation, 
        Expression* operand0, Expression* operand1);
    virtual std::int64_t evaluate(Context& context) override;
};

enum class Unary {
    Negate,
    BinNot,
    Not,
};

class UnaryExpression : public Expression {
private:
    Unary operation;
    Expression* operand;
public:
    UnaryExpression(Location location, Unary operation, Expression* operand);
    virtual std::int64_t evaluate(Context& context) override;
};

class SymbolicExpression : public Expression {
private:
public:
    Identifier identifier;

    SymbolicExpression(Location location, Identifier identifier);
    virtual std::int64_t evaluate(Context& context) override;
};

class LiteralExpression : public Expression {
private:
    std::int64_t value;
public:
    LiteralExpression(Location location, std::int64_t value);
    virtual std::int64_t evaluate(Context& context) override;
};

#endif

