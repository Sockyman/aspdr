#ifndef STATEMENT_HPP
#define STATEMENT_HPP

#include "Expression.hpp"
#include "Identifier.hpp"
#include "Location.hpp"
#include "Result.hpp"
#include "DataElement.hpp"
#include <SpdrFirmware/Instruction.hpp>
#include <SpdrFirmware/Mode.hpp>
#include <string>
#include <array>

class Context;

class Statement {
public:
    const Location location;

    Statement();
    Statement(Location location);
    virtual VoidResult assemble(Context& context) = 0;
};

class LabelStatement : public Statement {
public:
    const UnqualifiedIdentifier id;

    LabelStatement();
    LabelStatement(Location location, UnqualifiedIdentifier id);

    virtual VoidResult assemble(Context& context) override;
};

class SymbolStatement : public Statement {
public:
    const UnqualifiedIdentifier id;
    const Expression* expr;

    SymbolStatement();
    SymbolStatement(Location location, UnqualifiedIdentifier id, Expression* expr);

    virtual VoidResult assemble(Context& context) override;
};

class SectionStatement : public Statement {
private:
public:
    std::string sectionId;

    SectionStatement();
    SectionStatement(Location location, std::string sectionId);
    virtual VoidResult assemble(Context& context) override;
};

class AddressStatement : public Statement {
public:
    Expression* expr;

    AddressStatement();
    AddressStatement(Location location, Expression* expr);
    virtual VoidResult assemble(Context& context) override;
};

class AlignStatement : public Statement {
public:
    Expression* expr;

    AlignStatement();
    AlignStatement(Location location, Expression* expr);
    virtual VoidResult assemble(Context& context) override;
};

class ReserveStatement : public Statement {
public:
    Expression* expr;

    ReserveStatement();
    ReserveStatement(Location location, Expression* expr);
    virtual VoidResult assemble(Context& context) override;
};

class DataStatement : public Statement {
public:
    std::vector<DataElement*> elements;
    int defaultSize;

    DataStatement();
    DataStatement(Location location, std::vector<DataElement*> elements, int defaultSize = 1);

    virtual VoidResult assemble(Context& context) override;
};

class IncludeStatement : public Statement {
public:
    enum class Type {
        Assembly,
        Binary,
    };

    IncludeStatement::Type type;
    std::string fileName;

    IncludeStatement();
    IncludeStatement(Location location, IncludeStatement::Type type, std::string fileName);
    virtual VoidResult assemble(Context& context) override;
};

#endif

