#include "Statement.hpp"
#include "Assembler.hpp"
#include "Error.hpp"
#include "Context.hpp"
#include "Block.hpp"

int Statement::statementIdCounter = 0;

Statement::Statement() : Statement{Location{}} {}

Statement::Statement(Location location)
: location{location}, statementId{Statement::statementIdCounter++} {}

Statement::~Statement() {}


LabelStatement::LabelStatement(Location location, UnqualifiedIdentifier id)
: Statement{location}, id{id} {}

bool assembleLabel(
    Context& context,
    const Location& location,
    const UnqualifiedIdentifier& id
) {
    auto address = context.getSection().getAddress();
    if (!address) {
        context.error(
            Error::Level::Pass,
            "cannot get address",
            location
        );
        return false;
    }

    auto qualifiedId = context.qualify(location, id);

    if (!qualifiedId.has_value()) {
        return false;
    }

    context.setScope(qualifiedId.value());

    return context.assembler->assignSymbol(
        context,
        location,
        qualifiedId,
        *address
    );
}

bool LabelStatement::assemble(Context& context) {
    return assembleLabel(context, this->location, this->id);
}


SymbolStatement::SymbolStatement(
    Location location,
    UnqualifiedIdentifier id,
    Expression* expr
) : Statement{location}, id{id}, expr{expr} {}

bool SymbolStatement::assemble(Context& context) {
    std::optional<std::int64_t> result = this->expr->evaluate(context);
    if (!result.has_value()) {
        return false;
    }

    auto qualifiedId = context.qualify(this->location, this->id);

    return context.assembler->assignSymbol(
        context, this->location, qualifiedId, result.value()
    );
}

SymbolStatement::~SymbolStatement() {
    delete expr;
}


SectionStatement::SectionStatement(Location location, std::string sectionId)
: Statement{location}, sectionId{sectionId} {}

bool SectionStatement::assemble(Context& context) {
    return context.changeSection(this->location, this->sectionId);
}


AddressStatement::AddressStatement(Location location, Expression* expr)
    : Statement{location}, expr{expr} {}

bool AddressStatement::assemble(Context& context) {
    return context.getSection().changeAddress(context, this->expr);
}

AddressStatement::~AddressStatement() {
    delete expr;
}


AlignStatement::AlignStatement(Location location, Expression* expr)
    : Statement{location}, expr{expr} {}

bool AlignStatement::assemble(Context& context) {
    return context.getSection().align(context, this->expr);
}

AlignStatement::~AlignStatement() {
    delete expr;
}


ReserveStatement::ReserveStatement(Location location, Expression* expr)
: Statement{location}, expr{expr} {}

bool ReserveStatement::assemble(Context& context) {
    return context.getSection().reserve(context, this->expr);
}

ReserveStatement::~ReserveStatement() {
    delete expr;
}


DataStatement::DataStatement(
    Location location,
    std::vector<DataElement*> elements,
    int defaultSize
) : Statement{location}, elements{elements}, defaultSize{defaultSize} {}

bool DataStatement::assemble(Context& context) {
    for (auto& elem : this->elements) {
        elem->write(context, this->defaultSize);
    }
    return true;
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

bool IncludeStatement::assemble(Context& context) {
    if (type == IncludeStatement::Type::Assembly) {
        return context.assembler->assemble(
            context,
            this->fileName,
            this->location
        );
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


VariableStatement::VariableStatement(
    Location location,
    UnqualifiedIdentifier id,
    Expression* expr
): Statement{location}, id{id}, expr{expr} {}

VariableStatement::~VariableStatement() {
    delete this->expr;
}

bool VariableStatement::assemble(Context& context) {
    auto s = context.currentSection;
    context.changeSection(this->location, "var");
    assembleLabel(context, this->location, this->id);
    context.getSection().reserve(context, this->expr);
    return context.changeSection(this->location, s);
}


ProvidesStatement::ProvidesStatement(Location location, std::string fileName)
: Statement{location}, fileName{fileName} {
}

bool ProvidesStatement::assemble(Context& context) {
    return !context.markAsIncluded(this->fileName);
}

ConditionalStatement::ConditionalStatement(
    Location location,
    Expression* condition,
    Block* body,
    std::optional<Block*> elseBody
) : Statement{location}, condition{condition}, body{body}, elseBody{elseBody} {}

bool ConditionalStatement::assemble(Context& context) {
    auto value = this->condition->mustEvaluate(context);
    if (!value.has_value()) {
        return false;
    }

    if (value.value()) {
        this->body->assemble(context);
    } else if (elseBody.has_value()) {
        this->elseBody.value()->assemble(context);
    }

    return true;
}

RepeatStatement::RepeatStatement(
    Location location,
    Expression* times,
    Block* body,
    std::optional<std::string> counter
) : Statement{location}, times{times}, body{body}, counter{counter} {}

bool RepeatStatement::assemble(Context& context) {
    auto value = this->times->mustEvaluate(context);
    if (!value.has_value()) {
        return false;
    }

    context.frames.push(Frame{Frame::Type::Loop, this->statementId});

    for (int i = 0; i < value.value(); ++i) {
        context.frames.push(Frame{Frame::Type::Index, i});

        if (this->counter) {
            UnqualifiedIdentifier uid{0, Identifier{{"LOCAL", *this->counter}}};
            std::optional<Identifier> id{context.qualify(this->location, uid)};
            if (id) {
                context.assembler->assignSymbol(context, this->location, *id, i);
            }
        }

        this->body->assemble(context);
        context.frames.pop();
    }

    context.frames.pop();

    return true;
}
