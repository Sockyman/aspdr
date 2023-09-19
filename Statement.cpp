#include "Statement.hpp"
#include "Assembler.hpp"
#include "Error.hpp"
#include "Context.hpp"
#include <stdexcept>
#include <filesystem>

int Statement::statementIdCounter = 0;

Statement::Statement() : Statement{Location{}} {}

Statement::Statement(Location location)
: location{location}, statementId{Statement::statementIdCounter++} {}

Statement::~Statement() {}


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

SymbolStatement::~SymbolStatement() {
    delete expr;
}


SectionStatement::SectionStatement(Location location, std::string sectionId)
: Statement{location}, sectionId{sectionId} {}

VoidResult SectionStatement::assemble(Context& context) {
    return context.changeSection(this->location, this->sectionId);
}


AddressStatement::AddressStatement(Location location, Expression* expr)
    : Statement{location}, expr{expr} {}
VoidResult AddressStatement::assemble(Context& context) {
    return context.getSection().changeAddress(context, this->expr);
}

AddressStatement::~AddressStatement() {
    delete expr;
}


AlignStatement::AlignStatement(Location location, Expression* expr)
    : Statement{location}, expr{expr} {}

VoidResult AlignStatement::assemble(Context& context) {
    return context.getSection().align(context, this->expr);
}

AlignStatement::~AlignStatement() {
    delete expr;
}


ReserveStatement::ReserveStatement(Location location, Expression* expr)
: Statement{location}, expr{expr} {}

VoidResult ReserveStatement::assemble(Context& context) {
    return context.getSection().reserve(context, this->expr);
}

ReserveStatement::~ReserveStatement() {
    delete expr;
}


DataStatement::DataStatement(Location location, std::vector<DataElement*> elements, int defaultSize)
: Statement{location}, elements{elements}, defaultSize{defaultSize} {}

VoidResult DataStatement::assemble(Context& context) {
    auto result = VoidResult{};
    for (auto& elem : this->elements) {
        elem->write(context, this->defaultSize);
    }
    return result;
}

DataStatement::~DataStatement() {
    for (auto& elem : elements) {
        delete elem;
    }
}


IncludeStatement::IncludeStatement(
    Location location,
    IncludeStatement::Type type,
    std::string fileName
) : Statement{location}, type{type}, fileName{fileName} {}

VoidResult IncludeStatement::assemble(Context& context) {
    if (type == IncludeStatement::Type::Assembly) {
        return context.assembler->assemble(context, this->fileName, this->location);
    }

    /*if (context.assembler->binaryFiles.contains(this->fileName)) {
        context.getSection().writeBytes(
            context,
            this->location,
            context.assembler
            ->binaryFiles.at(this->fileName)
        );
    }

    auto file = context.assembler->openFile(context, this->fileName, this->location);
    if (file.isErr()) {
        return file.intoVoid();
    }*/
    ASSEMBLER_ERROR("Not implemented");
}


MacroStatement::MacroStatement(Location location, std::string name, std::vector<Statement*> statements)
    : name{name}, statements{statements} {}

MacroStatement::~MacroStatement() {
    for (auto& statement : this->statements) {
        delete statement;
    }
}

VoidResult MacroStatement::assemble(Context& context) {
    context.frames.push({Frame::Type::Macro, this->statementId});
    for (auto& statement : this->statements) {
        statement->assemble(context);
    }
    context.frames.pop();


    return {};
}

