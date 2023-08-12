#include "Statement.hpp"
#include "Assembler.hpp"
#include "Error.hpp"
#include "Context.hpp"
#include <stdexcept>
#include <filesystem>

Statement::Statement() : location{} {}

Statement::Statement(Location location) : location{location} {}


LabelStatement::LabelStatement() {}

LabelStatement::LabelStatement(Location location, UnqualifiedIdentifier id)
: Statement{location}, id{id} {}

VoidResult LabelStatement::assemble(Context& context) {
    auto address = context.getSection().getAddress();
    if (!address) {
        return context.voidError({
            Error::Level::Pass,
            this->location,
            "cannot get address"
        });
    }

    auto qualifiedId = context.qualify(this->location, this->id);

    if (qualifiedId.isErr()) {
        return qualifiedId.intoVoid();
    }

    context.setScope(qualifiedId.getOk());

    return context.assembler->assignSymbol(
        context,
        this->location,
        qualifiedId,
        *address
    );
}

SymbolStatement::SymbolStatement() {}

SymbolStatement::SymbolStatement(
    Location location,
    UnqualifiedIdentifier id,
    Expression* expr
) : Statement{location}, id{id}, expr{expr} {}

VoidResult SymbolStatement::assemble(Context& context) {
    Result<std::int64_t> result = this->expr->evaluate(context);
    if (result.isErr()) {
        return result.intoVoid();
    }

    auto qualifiedId = context.qualify(this->location, this->id);

    return context.assembler->assignSymbol(
        context, this->location, qualifiedId, result.getOk()
    );
}


SectionStatement::SectionStatement() {}
SectionStatement::SectionStatement(Location location, std::string sectionId)
: Statement{location}, sectionId{sectionId} {}

VoidResult SectionStatement::assemble(Context& context) {
    return context.changeSection(this->location, this->sectionId);
}

AddressStatement::AddressStatement() {}
AddressStatement::AddressStatement(Location location, Expression* expr)
    : Statement{location}, expr{expr} {}
VoidResult AddressStatement::assemble(Context& context) {
    return context.getSection().changeAddress(context, this->expr);
}

AlignStatement::AlignStatement() {}
AlignStatement::AlignStatement(Location location, Expression* expr)
    : Statement{location}, expr{expr} {}

VoidResult AlignStatement::assemble(Context& context) {
    return context.getSection().align(context, this->expr);
}

ReserveStatement::ReserveStatement() {}
ReserveStatement::ReserveStatement(Location location, Expression* expr)
: Statement{location}, expr{expr} {}

VoidResult ReserveStatement::assemble(Context& context) {
    return context.getSection().reserve(context, this->expr);
}

DataStatement::DataStatement() {}
DataStatement::DataStatement(Location location, std::vector<DataElement*> elements, int defaultSize)
: Statement{location}, elements{elements}, defaultSize{defaultSize} {}

VoidResult DataStatement::assemble(Context& context) {
    auto result = VoidResult{};
    for (auto& elem : this->elements) {
        elem->write(context, this->defaultSize);
    }
    return result;
}


IncludeStatement::IncludeStatement() {}
IncludeStatement::IncludeStatement(
    Location location,
    IncludeStatement::Type type,
    std::string fileName
) : Statement{location}, type{type}, fileName{fileName} {}

VoidResult IncludeStatement::assemble(Context& context) {
    return context.assembler->assemble(context, this->fileName, this->location);
}

