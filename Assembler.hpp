#ifndef ASSEMBLER_HPP
#define ASSEMBLER_HPP

#include "Statement.hpp"
#include "Identifier.hpp"
#include "Section.hpp"
#include <SpdrFirmware/InstructionSet.hpp>
#include <cstdint>
#include <vector>
#include <map>
#include <string>
#include <optional>

class Assembler {
private:
    const std::vector<Statement*> statements;
    std::map<Identifier, std::int64_t> symbols;
public:
    const InstructionSet instructionSet;

    Assembler(const std::vector<Statement*> statements);

    Result<Context> pass();
    Result<Context> passes();
    void assemble();

    std::optional<std::int64_t> resolveSymbol(const Identifier& identifier) const;
    VoidResult assignSymbol(const Location& location, const Identifier& identifier, std::int64_t value);
};

class Context {
private:
public:
    Assembler* assembler;
    Section section;
    //Section& getSection();

    Context(Assembler* assembler);
};

#endif

