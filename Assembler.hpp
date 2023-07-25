#ifndef ASSEMBLER_HPP
#define ASSEMBLER_HPP

#include "Statement.hpp"
#include "Identifier.hpp"
#include <SpdrFirmware/InstructionSet.hpp>
#include <cstdint>
#include <vector>
#include <optional>
#include <map>
#include <stack>
#include <string>

class Section {
private:
public:
    std::vector<std::uint8_t> bytes;
    std::size_t start;
    std::size_t offset;

    bool writable;
    bool counting;

    Section(std::size_t start, bool isWritable);

    void writeInteger(std::int64_t value, int number);
    void writeByte(std::uint8_t value);
    void writeWord(std::int64_t value);
    void reserve(std::size_t number);
    void changeAddress(std::int64_t address);

    std::int64_t getAddress();
};

class Assembler {
private:
    std::vector<Statement*> statements;
    std::map<Identifier, std::int64_t> symbols;
public:
    InstructionSet instructionSet;

    Assembler(std::vector<Statement*> statements);

    void assemble();
    bool resolveSymbol(const Identifier& identifier, std::int64_t& value) const;
    void assignSymbol(const Identifier& identifier, std::int64_t value);
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

