#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include "Error.hpp"
#include "ErrorHandler.hpp"
#include "Section.hpp"
#include "Assembler.hpp"
#include <map>
#include <string>
#include <set>
#include <vector>

class Context : public ErrorHandler {
private:
public:
    Assembler* assembler;

    std::vector<Error> errors;
    std::map<std::string, Section> sections;
    std::string currentSection;
    std::set<std::string> includedFiles;

    Identifier scope;

    Context(Assembler* assembler);

    virtual std::vector<Error>& getErrors() override;
    virtual const std::vector<Error>& getErrors() const override;

    Section& getSection();
    VoidResult changeSection(
        const Location& location,
        const std::string& newSection
    );

    Result<Identifier> qualify(
        const Location& location,
        const UnqualifiedIdentifier& unqualified
    );

    void setScope(const Identifier& id);

    bool markAsIncluded(const std::string& fileName);
};

#endif

