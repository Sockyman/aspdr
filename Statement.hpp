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
private:
    static int statementIdCounter;
public:
    const Location location;
    const int statementId;

    Statement();
    Statement(Location location);
    virtual VoidResult assemble(Context& context) = 0;

    virtual ~Statement();
};

class LabelStatement : public Statement {
public:
    const UnqualifiedIdentifier id;

    //LabelStatement();
    LabelStatement(Location location, UnqualifiedIdentifier id);

    virtual VoidResult assemble(Context& context) override;
};

class SymbolStatement : public Statement {
public:
    const UnqualifiedIdentifier id;
    const Expression* expr;

    //SymbolStatement();
    SymbolStatement(Location location, UnqualifiedIdentifier id, Expression* expr);

    virtual VoidResult assemble(Context& context) override;

    virtual ~SymbolStatement() override;
};

class SectionStatement : public Statement {
private:
public:
    std::string sectionId;

    //SectionStatement();
    SectionStatement(Location location, std::string sectionId);
    virtual VoidResult assemble(Context& context) override;
};

class AddressStatement : public Statement {
public:
    Expression* expr;

    //AddressStatement();
    AddressStatement(Location location, Expression* expr);
    virtual VoidResult assemble(Context& context) override;

    virtual ~AddressStatement() override;
};

class AlignStatement : public Statement {
public:
    Expression* expr;

    //AlignStatement();
    AlignStatement(Location location, Expression* expr);
    virtual VoidResult assemble(Context& context) override;

    virtual ~AlignStatement() override;
};

class ReserveStatement : public Statement {
public:
    Expression* expr;

    //ReserveStatement();
    ReserveStatement(Location location, Expression* expr);
    virtual VoidResult assemble(Context& context) override;

    virtual ~ReserveStatement() override;
};

class DataStatement : public Statement {
public:
    std::vector<DataElement*> elements;
    int defaultSize;

    //DataStatement();
    DataStatement(Location location, std::vector<DataElement*> elements, int defaultSize = 1);

    virtual VoidResult assemble(Context& context) override;

    virtual ~DataStatement() override;
};

class IncludeStatement : public Statement {
public:
    enum class Type {
        Assembly,
        Binary,
    };

    IncludeStatement::Type type;
    std::string fileName;

    //IncludeStatement();
    IncludeStatement(Location location, IncludeStatement::Type type, std::string fileName);
    virtual VoidResult assemble(Context& context) override;
};

class MacroStatement : public Statement {
public:
    std::string name;
    std::vector<Statement*> statements;

    MacroStatement(Location location, std::string name, std::vector<Statement*> statements);
    virtual ~MacroStatement() override;

    virtual VoidResult assemble(Context& context) override;
};

#endif

