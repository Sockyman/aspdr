#include "Expression.hpp"
#include "Error.hpp"
#include "Assembler.hpp"
#include <stdexcept>

Expression::Expression(Location location) : location{location} {
}

BinaryExpression::BinaryExpression(Location location, Binary operation, 
    Expression* operand0, Expression* operand1
) : Expression{location}, operation{operation}, operand0{operand0}, operand1{operand1} {}

std::int64_t BinaryExpression::evaluate(Context& context) {
    auto x = this->operand0->evaluate(context);
    auto y = this->operand1->evaluate(context);
    switch (this->operation) {
        case Binary::Add:
            return x + y;
        case Binary::Subtract:
            return x - y;
        case Binary::Multiply:
            return x * y;
        case Binary::Divide:
            if (y == 0)
                throw FatalError{this->location, "division by zero."};
            return x / y;
        case Binary::Modulo:
            if (y == 0)
                throw FatalError{this->location, "division by zero."};
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

UnaryExpression::UnaryExpression(Location location, Unary operation, Expression* operand)
    : Expression{location}, operation{operation}, operand{operand} {}

std::int64_t UnaryExpression::evaluate(Context& context) {
    throw std::logic_error(__PRETTY_FUNCTION__);
}

SymbolicExpression::SymbolicExpression(Location location, Identifier identifier)
    : Expression{location}, identifier{identifier} {}

std::int64_t SymbolicExpression::evaluate(Context& context) {
    std::int64_t value;
    if (context.assembler->resolveSymbol(this->identifier, value))
        return value;

    std::stringstream ss{};
    ss << "cannot resolve symbol.";
    throw PassError{ss.str()};
}

LiteralExpression::LiteralExpression(Location location, std::int64_t value)
: Expression{location}, value{value} {}

std::int64_t LiteralExpression::evaluate(Context& context) {
    return this->value;
}

