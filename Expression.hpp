#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include "Identifier.hpp"
#include "Location.hpp"
#include "Result.hpp"
#include <cstdint>
#include <memory>
#include <exception>

class Context;

class Expression {
private:
public:
    Location location;
    Expression(Location location);
    virtual Result<std::int64_t> evaluate(Context& context) const = 0;

    virtual ~Expression();
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

    Result<std::int64_t> performOperation(
        Context& context,
        std::int64_t x,
        std::int64_t y
    ) const;

public:
    BinaryExpression(Location location, Binary operation, 
        Expression* operand0, Expression* operand1);
    virtual Result<std::int64_t> evaluate(Context& context) const override;

    virtual ~BinaryExpression() override;
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
    virtual Result<std::int64_t> evaluate(Context& context) const override;

    virtual ~UnaryExpression() override;
};

class SymbolicExpression : public Expression {
private:
public:
    UnqualifiedIdentifier identifier;

    SymbolicExpression(Location location, UnqualifiedIdentifier identifier);
    virtual Result<std::int64_t> evaluate(Context& context) const override;
};

class LiteralExpression : public Expression {
private:
    std::int64_t value;
public:
    LiteralExpression(Location location, std::int64_t value);
    virtual Result<std::int64_t> evaluate(Context& context) const override;
};

#endif

