#include "Expression.hpp"
#include "Result.hpp"
#include "Error.hpp"
#include "Assembler.hpp"
#include "Context.hpp"
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

Expression::~Expression() {}

Result<std::int64_t> BinaryExpression::performOperation(
    Context& context,
    std::int64_t x,
    std::int64_t y
) const {
    switch (operation) {
        case Binary::Add:
            return x + y;
        case Binary::Subtract:
            return x - y;
        case Binary::Multiply:
            return x * y;
        case Binary::Divide:
            if (y == 0)
                return context.error<std::int64_t>(Error{
                    Error::Level::Fatal, location, "division by zero."});
            return x / y;
        case Binary::Modulo:
            if (y == 0)
                return context.error<std::int64_t>(Error{
                    Error::Level::Fatal, location, "division by zero."});
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

Result<std::int64_t> BinaryExpression::evaluate(Context& context) const {
    auto r0 = this->operand0->evaluate(context);
    auto r1 = this->operand1->evaluate(context);
    auto result0And1 = r0.then(r1);
    if (result0And1.isErr()) {
        return result0And1;
    }
    return this->performOperation(context, *r0, *r1);
}

BinaryExpression::~BinaryExpression() {
    delete this->operand0;
    delete this->operand1;
}


UnaryExpression::UnaryExpression(
    Location location,
    Unary operation,
    Expression* operand
)
    : Expression{location},
    operation{operation},
    operand{operand} {}

Result<std::int64_t> UnaryExpression::evaluate(Context& context) const {
    auto result = this->operand->evaluate(context);
    if (result.isErr()) {
        return result;
    }
    auto x = result.getOk();

    switch (this->operation) {
        case Unary::Negate:
            return -x;
        case Unary::Not:
            return !x;
        case Unary::BinNot:
            return ~x;
    }
    ASSEMBLER_ERROR("unsupported unary operator.");
}

UnaryExpression::~UnaryExpression() {
    delete this->operand;
}


SymbolicExpression::SymbolicExpression(Location location, UnqualifiedIdentifier identifier)
    : Expression{location}, identifier{identifier} {}

Result<std::int64_t> SymbolicExpression::evaluate(
    Context& context
) const {
    auto qualifiedId = context.qualify(this->location, this->identifier);
    auto symbol = context.assembler->resolveSymbol(qualifiedId);

    if (!symbol) {
        std::stringstream ss{};
        ss << "cannot resolve symbol \'" << this->identifier << "\'";
        return context.error<std::int64_t>(
            {Error::Level::Pass, this->location, ss.str()});
    }
    return Result{*symbol};
}

LiteralExpression::LiteralExpression(Location location, std::int64_t value)
: Expression{location}, value{value} {}

Result<std::int64_t> LiteralExpression::evaluate(Context& context) const {
    return Result{this->value};
}

