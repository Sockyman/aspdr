#include "Expression.hpp"
#include "Result.hpp"
#include "Error.hpp"
#include "Assembler.hpp"
#include <stdexcept>

Expression::Expression(Location location) : location{location} {
}

BinaryExpression::BinaryExpression(
    Location location,
    Binary operation, 
    Expression* operand0,
    Expression* operand1
)
    : Expression{location},
    operation{operation},
    operand0{operand0},
    operand1{operand1} {}

Result<std::int64_t> BinaryExpression::performOperation(const Location& location, Binary operation, std::int64_t x, std::int64_t y) {
    switch (operation) {
        case Binary::Add:
            return x + y;
        case Binary::Subtract:
            return x - y;
        case Binary::Multiply:
            return x * y;
        case Binary::Divide:
            if (y == 0)
                return {Error{Error::Level::Fatal, location, "division by zero."}};
            return x / y;
        case Binary::Modulo:
            if (y == 0)
                return {Error{Error::Level::Fatal, location, "division by zero."}};
            return x % y;
        case Binary::ShiftLeft:
            return x << y;
        case Binary::ShiftRight:
            return x >> y;
        case Binary::BinAnd:
            return x & y;
        case Binary::BinOr:
            return x | y;
        case Binary::BinXor:
            return x ^ y;
        case Binary::And:
            return x && y;
        case Binary::Or:
            return x || y;
        case Binary::Greater:
            return x > y;
        case Binary::Less:
            return x < y;
        case Binary::GreaterEqual:
            return x >= y;
        case Binary::LessEqual:
            return x <= y;
        case Binary::Equal:
            return x == y;
        case Binary::NotEqual:
            return x != y;
    }
    ASSEMBLER_ERROR("unsupported binary operator.");
}

Result<std::int64_t> BinaryExpression::evaluate(const Context& context) const {
    return this->operand0->evaluate(context)
        .then<std::int64_t>(this->operand1->evaluate(context),
            [this](std::int64_t x, std::int64_t y) {
                return performOperation(this->location, this->operation, x, y);
            });
}

UnaryExpression::UnaryExpression(
    Location location,
    Unary operation,
    Expression* operand
)
    : Expression{location},
    operation{operation},
    operand{operand} {}

Result<std::int64_t> UnaryExpression::evaluate(const Context& context) const {
    throw std::logic_error(__PRETTY_FUNCTION__);
}

SymbolicExpression::SymbolicExpression(Location location, Identifier identifier)
    : Expression{location}, identifier{identifier} {}

Result<std::int64_t> SymbolicExpression::evaluate(const Context& context) const {
    auto symbol = context.assembler->resolveSymbol(this->identifier);
    if (!symbol) {
        std::stringstream ss{};
        ss << "cannot resolve symbol \'" << this->identifier << "\'";
        return Result<std::int64_t>{{Error::Level::Pass, this->location, ss.str()}};
    }
    return Result{*symbol};
}

LiteralExpression::LiteralExpression(Location location, std::int64_t value)
: Expression{location}, value{value} {}

Result<std::int64_t> LiteralExpression::evaluate(const Context& context) const {
    return Result{this->value};
}

