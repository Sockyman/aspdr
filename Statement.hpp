#ifndef STATEMENT_HPP
#define STATEMENT_HPP

#include "Expression.hpp"
#include "Identifier.hpp"
#include "Location.hpp"
#include "DataElement.hpp"
#include <SpdrFirmware/Instruction.hpp>
#include <SpdrFirmware/Mode.hpp>
#include <string>

class Context;
class Block;

class Statement {
private:
    static int statementIdCounter;
public:
    const Location location;
    const int statementId;

    Statement();
    Statement(Location location);
    virtual bool assemble(Context& context) = 0;

    virtual ~Statement();
};


class LabelStatement : public Statement {
public:
    const UnqualifiedIdentifier id;

    //LabelStatement();
    LabelStatement(Location location, UnqualifiedIdentifier id);

    virtual bool assemble(Context& context) override;
};

class SymbolStatement : public Statement {
public:
    const UnqualifiedIdentifier id;
    const Expression* expr;

    //SymbolStatement();
    SymbolStatement(Location location, UnqualifiedIdentifier id, Expression* expr);

    virtual bool assemble(Context& context) override;

    virtual ~SymbolStatement() override;
};

class SectionStatement : public Statement {
private:
public:
    std::string sectionId;

    //SectionStatement();
    SectionStatement(Location location, std::string sectionId);
    virtual bool assemble(Context& context) override;
};

class AddressStatement : public Statement {
public:
    Expression* expr;

    //AddressStatement();
    AddressStatement(Location location, Expression* expr);
    virtual bool assemble(Context& context) override;

    virtual ~AddressStatement() override;
};

class AlignStatement : public Statement {
public:
    Expression* expr;

    //AlignStatement();
    AlignStatement(Location location, Expression* expr);
    virtual bool assemble(Context& context) override;

    virtual ~AlignStatement() override;
};

class ReserveStatement : public Statement {
public:
    Expression* expr;

    //ReserveStatement();
    ReserveStatement(Location location, Expression* expr);
    virtual bool assemble(Context& context) override;

    virtual ~ReserveStatement() override;
};

class DataStatement : public Statement {
public:
    std::vector<DataElement*> elements;
    int defaultSize;

    //DataStatement();
    DataStatement(Location location, std::vector<DataElement*> elements, int defaultSize = 1);

    virtual bool assemble(Context& context) override;

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
    virtual bool assemble(Context& context) override;
};

class VariableStatement : public Statement {
public:
    UnqualifiedIdentifier id;
    Expression* expr;

    VariableStatement(Location, UnqualifiedIdentifier id, Expression* expr);
    virtual ~VariableStatement() override;

    virtual bool assemble(Context& context) override;
};


class ProvidesStatement : public Statement {
public:
    std::string fileName;

    ProvidesStatement(Location location, std::string fileName);
    virtual bool assemble(Context& context) override;
};

class ConditionalStatement : public Statement {
public:
    Expression* condition;
    Block* body;
    std::optional<Block*> elseBody;

    ConditionalStatement(
        Location location,
        Expression* condition,
        Block* body,
        std::optional<Block*> elseBody = std::nullopt
    );

    virtual bool assemble(Context& context) override;
};

class RepeatStatement : public Statement {
public:
    Expression* times;
    Block* body;
    std::optional<std::string> counter;

    RepeatStatement(
        Location location,
        Expression* times,
        Block* body,
        std::optional<std::string> counter = std::nullopt
    );

    virtual bool assemble(Context& context) override;
};

template<typename T>
Instruction getInstruction(std::string name, std::vector<std::pair<Address, T>> elements) {
    std::vector<SizedAddress> mode{};
    for (const auto& elem : elements) {
        mode.push_back(elem.first);
    }

    return Instruction{name, AddressingMode{mode}};
}

template<typename T, typename U>
std::vector<U> getSecond(const std::vector<std::pair<T, U>>& input) {
    std::vector<U> output;
    for (const std::pair<T, U>& elem : input) {
        output.push_back(elem.second);
    }
    return output;
}

#endif

