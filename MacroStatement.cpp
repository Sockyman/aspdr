#include "MacroStatement.hpp"
#include "Context.hpp"
#include "Block.hpp"
#include "Error.hpp"
#include <SpdrFirmware/Instruction.hpp>

MacroStatement::MacroStatement(
    Location location,
    std::string name,
    std::vector<std::pair<Address, std::optional<std::string>>> parameters,
    Block* block
) : Statement{location}, name{name}, parameters{parameters}, block{block} {}

MacroStatement::~MacroStatement() {
    delete block;
}

bool MacroStatement::assemble(Context& context) {
    return context.addMacro(this);
}

bool MacroStatement::assembleBlock(
    Context& context,
    const std::vector<Expression*>& arguments,
    int id
) {
    ASSEMBLER_ASSERT(
        this->parameters.size() == arguments.size(),
        "parameter count does not match passed argument count"
    );

    context.frames.push({Frame::Type::Macro, id});

    for (std::size_t i = 0; i < arguments.size(); ++i) {
        if (!parameters[i].second.has_value()) {
            continue;
        }

        UnqualifiedIdentifier uid{0, Identifier{{"MACRO", parameters[i].second.value()}}};
        std::optional<Identifier> id{context.qualify(this->location, uid)};
        if (!id) {
            continue;
        }

        std::optional<std::int64_t> val = arguments[i]->evaluate(context);
        if (!val) {
            continue;
        }

        context.assembler->assignSymbol(context, this->location, id, *val);
    }

    this->block->assemble(context);
    context.frames.pop();
    return true;
}

Instruction MacroStatement::getInstruction() {
    return ::getInstruction(this->name, this->parameters);
}

